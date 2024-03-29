/**
 * TCPIPThreadReceiver.cpp
 *
 *  Created on: 11 wrz 2016
 *      Author: Kamil Burzy�ski
 */

#include <Threads/TCPIP/TCPIPThreadReceiver.hpp>

const int thd::TCPIPThreadReceiver::GAME_LISTEN_PORT = 3097;

void thd::TCPIPThreadReceiver::receiverInit() {
#ifdef __WIN32__
	WORD versionWanted = MAKEWORD(1, 1);
	WSADATA wsaData;
	WSAStartup(versionWanted, &wsaData);
#endif
	std::memset(&receiverIn_, 0, sizeof(receiverIn_));
	std::memset(&client_, 0, sizeof(client_));
	std::memset(buffer_, 0, sizeof(buffer_));

	if ((receiverFd_ = socket(AF_INET, SOCK_TYPE, 0)) < 0)
		gameAssert(!"socked failed");

	if (setsockopt(receiverFd_, SOL_SOCKET, SO_REUSEADDR, &optval_,
			sizeof(optval_)) < 0)
		gameAssert(!"setsockopt failed");

	//Ustawianie sockaddr_in
	receiverIn_.sin_family = AF_INET;
	receiverIn_.sin_port = htons(GAME_LISTEN_PORT);
	receiverIn_.sin_addr.s_addr = INADDR_ANY; //Przyjmuj wszystkie adresy.

	if (bind(receiverFd_, (struct sockaddr*) &receiverIn_,
			sizeof(struct sockaddr)) < 0)
		gameAssert(!"bind failed");

	//http://tangentsoft.net/wskfaq/advanced.html#backlog
	if (listen(receiverFd_, SOMAXCONN) < 0)
		gameAssert(!"listen failed");

	sizeSockaddrIn_ = sizeof(struct sockaddr_in);
	closeFlag_ = false;
	socketFlags_ = 0;

	if (!setBlockingMode(receiverFd_, false))
		gameAssert(!"Failed to set nonblocking mode");
}

bool thd::TCPIPThreadReceiver::setBlockingMode(const int& socket,
		const bool blocking) {
#ifdef __WIN32__
	unsigned long mode = blocking ? 0 : 1;
	if (ioctlsocket(socket, FIONBIO, &mode) == 0) {
		blockingFlag_ = blocking;
		return true;
	}
	return false;
#else
	int flags = fcntl(socket, F_GETFL, 0);
	if (flags < 0)
	return false;
	flags = blocking ? (flags&~O_NONBLOCK) : (flags|O_NONBLOCK);
	if(fcntl(socket, F_SETFL, flags) == 0)) {
		blockingFlag_ = blocking;
		return true;
	}
	return false;
#endif
}

void thd::TCPIPThreadReceiver::processInput(boost::any elem) {
	try {
		boost::any_cast<boost::none_t>(elem);
	} catch (boost::bad_any_cast &e) {
		try {
			boost::any_cast<mtfifo::ExitThread>(elem);
			closeFlag_ = true;
		} catch (boost::bad_any_cast &e) {
			gameAssert(!"Unknown element type");
		}
	}
}

void thd::TCPIPThreadReceiver::operator()() {
	receiverInit();
	mtfifo::FIFODistributor& fifoDistributor =
			mtfifo::FIFODistributor::getInstance();
	mtfifo::FIFO<mtfifo::FIFOInput> input = fifoDistributor.getFIFO<
			mtfifo::FIFOInput>(mtfifo::FIFO_TCPIP_REC_EXIT);
	mtfifo::FIFO<mtfifo::FIFOOutput> output = fifoDistributor.getFIFO<
			mtfifo::FIFOOutput>(mtfifo::FIFO_SERIALIZED_INPUT);
	mtfifo::FIFO<mtfifo::FIFOOutput> log = fifoDistributor.getFIFO<
			mtfifo::FIFOOutput>(mtfifo::FIFO_LOG);

	boost::thread::id id = boost::this_thread::get_id();
	thd::ThreadRegistration& threadRegistration =
			thd::ThreadRegistration::getInstance();
	threadRegistration.registerThread(id, thd::TCPIP_RECEIVER);

	boost::any outputElem;

	boost::random::mt19937 rng;
	boost::random::uniform_int_distribution<> ten(1, 10);

	mtfifo::LogElement logMsg = mtfifo::LogElement();
	logMsg << id;

	while (1) {
		processInput(input.get()); //Sprawdzenie czy wydano sygna� zako�czenia w�tku
		if (closeFlag_)
			break;

		logMsg << normal;
		logMsg << "Czekam na wykonanie si� accept";
		log << boost::any(logMsg);

		if ((realReceiverFd_ = accept(receiverFd_, (struct sockaddr*) &client_,
				&sizeSockaddrIn_)) < 0) {
			logMsg << notification;
			logMsg << "accept nie zwr�ci�o poprawnej warto�ci";
			log << boost::any(logMsg);
			boost::this_thread::sleep_for(TCPIP_RECEIVER_TIME);
			continue;
		}

		if (!setBlockingMode(realReceiverFd_, false))
			gameAssert(!"Failed to set nonblocking mode, 2nd");

		logMsg << normal;
		logMsg
				<< "accept wykonane poprawnie. Zwr�cono: "
						+ boost::lexical_cast<std::string>(realReceiverFd_);
		log << boost::any(logMsg);

		while (1) {
			processInput(input.get()); //Sprawdzenie czy wydano sygna� zako�czenia w�tku
			if (closeFlag_)
				break;

			logMsg << normal;
			logMsg
					<< "Czekam na dane z socketa: "
							+ boost::lexical_cast<std::string>(realReceiverFd_)
							+ " na porcie: "
							+ boost::lexical_cast<std::string>(GAME_LISTEN_PORT);
			log << logMsg;

			recvResult_ = recv(realReceiverFd_, buffer_, BUFFER_SIZE,
					socketFlags_);

			if (blockingFlag_) {
				setBlockingMode(realReceiverFd_, false);
				logMsg << critical;
				logMsg << "Pr�ba ponownego prze��czenia w tryb nieblokuj�cy";
				log << logMsg;
			}

			if (recvResult_ <= -1) {
				logMsg << critical;
				logMsg << "recv zwr�ci�o warto�c ujemn�: ";
				log << logMsg;
				//TODO przerzucic do oddzielnej funkcji, zwracaj�cej stringa
#ifdef __WIN32__
				// https://msdn.microsoft.com/pl-pl/library/windows/desktop/ms740121(v=vs.85).aspx
				// https://msdn.microsoft.com/pl-pl/library/windows/desktop/ms741580(v=vs.85).aspx
				// http://en.cppreference.com/w/cpp/string/multibyte/wcstombs
				// http://stackoverflow.com/questions/3400922/how-do-i-retrieve-an-error-string-from-wsagetlasterror
				int errNumber = WSAGetLastError(); //Numer b��du socketa
				wchar_t *errorTxt = NULL;
				FormatMessageW(
						FORMAT_MESSAGE_ALLOCATE_BUFFER
								| FORMAT_MESSAGE_FROM_SYSTEM
								| FORMAT_MESSAGE_IGNORE_INSERTS,
						NULL, errNumber,
						MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
						(LPWSTR) &errorTxt, 0, NULL); //Wyci�gni�cie wiadomo�ci do wchar_t
				char errorNameBuffer[BUFFER_SIZE];
				std::memset(errorNameBuffer, 0, sizeof(errorNameBuffer));
				std::wcstombs(errorNameBuffer, errorTxt, BUFFER_SIZE); //Konwersja do char*
				logMsg << critical;
				logMsg
						<< boost::lexical_cast<std::string>(errNumber)
								+ " oznaczaj�c�: "
								+ std::string(errorNameBuffer);
				LocalFree(errorTxt); //Konieczne ze wzgl�du na mo�liwy wyciek pami�ci
#else
						// http://man7.org/linux/man-pages/man2/recv.2.html
						recvErrorMsg += boost::lexical_cast<std::string>(errno) + " oznaczaj�c�: "
						+ std::string(strerror(errno));
						log << logMsg;
#endif
				if ((errNumber == WSAEWOULDBLOCK)
						|| ((errno == EWOULDBLOCK) || (errno == EAGAIN))) {
					logMsg << critical;
					logMsg << "Prze��czam w tryb blokuj�cy."; //Wiadomo�c do loga
					log << logMsg;
					setBlockingMode(realReceiverFd_, true);
				} else {
					close(realReceiverFd_);
					break;
				}
			} else if (recvResult_ == 0) {
				logMsg << notification;
				logMsg << "recv zwr�ci�o zako�czenie po��czenia";
				log << logMsg;
				close(realReceiverFd_);
				break;
			} else {
				buffer_[recvResult_] = '\0';
				std::string buffer(buffer_);
				if (ten(rng) > 0) { //Zawsze prawdziwe
					logMsg << normal;
					logMsg << "Odczytano dane: " + buffer;
					log << logMsg;
				}
				outputElem = mtfifo::TCPIPSerialized(buffer);
				output.put(outputElem);
			}
			boost::this_thread::sleep_for(TCPIP_RECEIVER_TIME);
		} //while
		close(realReceiverFd_);
		if (closeFlag_)
			break;
		boost::this_thread::sleep_for(TCPIP_RECEIVER_TIME * 2);
	} //while
}

thd::TCPIPThreadReceiver::~TCPIPThreadReceiver() {
}
