/**
 * TCPIPThreadSender.cpp
 *
 *  Created on: 11 wrz 2016
 *      Author: Kamil Burzyñski
 */

#include <Threads/TCPIP/TCPIPThreadSender.hpp>

const int thd::TCPIPThreadSender::GAME_SEND_PORT = 3098;
const int thd::TCPIPThreadSender::MAX_ERROR_COUNTER = 15; //Arbitralnie dobrana wielkoœc
const bool thd::TCPIPThreadSender::BLOCKING_MODE = true;
const char* SERVER_ADDRESS = "127.0.0.1";

void thd::TCPIPThreadSender::senderInit() {
#ifdef __WIN32__
	WORD versionWanted = MAKEWORD(1, 1);
	WSADATA wsaData;
	WSAStartup(versionWanted, &wsaData);
#endif
	std::memset(&ipv4addr_, 0, sizeof(ipv4addr_));
#ifdef __WIN32__
	inet_pton(AF_INET, SERVER_ADDRESS, &ipv4addr_);
#else
	inet_aton(SERVER_ADDRESS, &ipv4addr_);
#endif

	if ((host_ = gethostbyaddr((const char *) &ipv4addr_,
			sizeof(struct in_addr), AF_INET)) == NULL)
		gameAssert(!"gethostbyaddr failed");

	if ((socketFd_ = socket(AF_INET, SOCK_TYPE, 0)) < 0)
		gameAssert(!"socket failed");

	if (!setBlockingMode(socketFd_, BLOCKING_MODE))
		gameAssert("setBlockingMode failed, 2nd");

	std::memset(&server_, 0, sizeof(server_));
	server_.sin_family = AF_INET;
	server_.sin_port = htons(GAME_SEND_PORT);
	server_.sin_addr = *((struct in_addr *) host_->h_addr);

	closeFlag_ = false;
}

bool thd::TCPIPThreadSender::setBlockingMode(const int& socket,
		const bool blocking) {
#ifdef __WIN32__
	unsigned long mode = blocking ? 0 : 1;
	if (ioctlsocket(socket, FIONBIO, &mode) == 0)
		return true;
	return false;
#else
	int flags = fcntl(socket, F_GETFL, 0);
	if (flags < 0)
	return false;
	flags = blocking ? (flags&~O_NONBLOCK) : (flags|O_NONBLOCK);
	if(fcntl(socket, F_SETFL, flags) == 0))
	return true;
	return false;
#endif
}

void thd::TCPIPThreadSender::processInput(boost::any elem) {
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

void thd::TCPIPThreadSender::operator()() {
	senderInit();
	mtfifo::FIFODistributor& fifoDistributor =
			mtfifo::FIFODistributor::getInstance();
	mtfifo::FIFO<mtfifo::FIFOInput> input = fifoDistributor.getFIFO<
			mtfifo::FIFOInput>(mtfifo::FIFO_TCPIP_SEN_EXIT);
	mtfifo::FIFO<mtfifo::FIFOInput> input2 = fifoDistributor.getFIFO<
			mtfifo::FIFOInput>(mtfifo::FIFO_SERIALIZED_OUTPUT);
	mtfifo::FIFO<mtfifo::FIFOOutput> log = fifoDistributor.getFIFO<
			mtfifo::FIFOOutput>(mtfifo::FIFO_LOG);

	boost::thread::id id = boost::this_thread::get_id();
	thd::ThreadRegistration& threadRegistration =
			thd::ThreadRegistration::getInstance();
	threadRegistration.registerThread(id, thd::TCPIP_SENDER);

	boost::any elem;

	boost::random::mt19937 rng;
	boost::random::uniform_int_distribution<> ten(1, 10);

	mtfifo::LogElement logMsg = mtfifo::LogElement();
	logMsg << id;

	while (1) {
		processInput(input.get());
		if (closeFlag_)
			break;

		if (connect(socketFd_, (struct sockaddr *) &server_,
				sizeof(struct sockaddr)) < 0) {
			logMsg << notification;
			logMsg << "connect zwróci³o wartoœc ujemn¹";
			log << logMsg;
			boost::this_thread::sleep_for(TCPIP_SENDER_TIME);
			continue;
		}

		errorCounter_ = 0;

		while (1) {
			processInput(input.get()); //Sprawdzenie czy wydano sygna³ zakoñczenia w¹tku
			if (closeFlag_)
				break;

			elem = input2.get();
			try {
				boost::any_cast<boost::none_t>(elem);
				boost::this_thread::sleep_for(TCPIP_SENDER_TIME);
				continue;
			} catch (boost::bad_any_cast &e) {
				try {
					mtfifo::TCPIPSerialized msgToSend = boost::any_cast<
							mtfifo::TCPIPSerialized>(elem);
					//Gwarancja, ¿e jest null terminated
					const char *message = msgToSend.serialized.c_str();
					if ((std::strlen(message) + 1) > BUFFER_SIZE)
						gameAssert(!"serialized message is too big");
					std::strcpy(buffer_, message);
				} catch (boost::bad_any_cast &e) {
					gameAssert(!"Unknown element type");
				}
			}
			if ((send(socketFd_, buffer_, strlen(buffer_), 0)) < 0) {
				logMsg << critical;
				logMsg << "send zwróci³o wartoœc ujemn¹";
				log << logMsg;
				++errorCounter_;
				boost::this_thread::sleep_for(TCPIP_SENDER_TIME);
				if (errorCounter_ < MAX_ERROR_COUNTER) {
					close(socketFd_);
					gameAssert(!"send failed");
				}
			} else {
				errorCounter_ = 0;
				logMsg << normal;
				logMsg << "send powiod³o siê, wys³ano: " + std::string(buffer_);
				log << logMsg;
			}
			boost::this_thread::sleep_for(TCPIP_SENDER_TIME);
		} //while
		boost::this_thread::sleep_for(TCPIP_SENDER_TIME * 2);
	} //while
	close(socketFd_);
}

thd::TCPIPThreadSender::~TCPIPThreadSender() {
}

