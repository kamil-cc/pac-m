/**
 * TCPIPThreadReceiver.hpp
 *
 *  Created on: 1 wrz 2016
 *      Author: Kamil Burzyñski
 */

#ifndef GAME_INCLUDES_THREADS_TCPIP_TCPIPTHREADRECEIVER_HPP_
#define GAME_INCLUDES_THREADS_TCPIP_TCPIPTHREADRECEIVER_HPP_

//Przenoœne podejœcie do socketów Windows/Linux
#ifdef __WIN32__
	#include <winsock2.h>
	#include <ws2tcpip.h>
	int close(int s) {return closesocket(s);}
#else
	#include <sys/socket.h>
#endif

namespace thd{
	//Konfiguracja po³¹czenia sieciowego
	const int GAME_LISTEN_PORT = 3097;
	const int BUFFER_SIZE = 1024;

	/**
	 * Klasa zawieraj¹ca obs³ugê wejœciowego stosu internetowego
	 */
	class TCPIPThreadReciever{
	public:
		void receiverInit(){//TODO do pliku cpp
#ifdef __WIN32__
			WORD versionWanted = MAKEWORD(1, 1);
			WSADATA wsaData;
			WSAStartup(versionWanted, &wsaData);
#endif
			std::memset(&receiverIn_, 0, sizeof(receiverIn_));
			std::memset(&client_, 0, sizeof(client_));
			std::memset(buffer_, 0, sizeof(buffer_)/sizeof(*buffer_));

			if((receiverFd_ = socket(AF_INET, SOCK_STREAM, 0)) < 0)
				assert(!"socked failed");

			if(setsockopt(receiverFd_, SOL_SOCKET, SO_REUSEADDR, &optval_, sizeof(optval_)) < 0)
				assert(!"setsockopt failed");

			//Ustawianie sockaddr_in
			receiverIn_.sin_family = AF_INET;
			receiverIn_.sin_port = htons(GAME_LISTEN_PORT);
			receiverIn_.sin_addr.s_addr = INADDR_ANY;

			if(bind(receiverFd_, (struct sockaddr*)&receiverIn_, sizeof(struct sockaddr)) < 0)
				assert(!"bind failed");

			//http://tangentsoft.net/wskfaq/advanced.html#backlog
			if(listen(receiverFd_, SOMAXCONN) < 0)
				assert(!"listen failed");

			sizeSockaddrIn_ = sizeof(struct sockaddr_in);
			closeFlag_ = false;
			socketFlags_ = 0;
		}

		void processInput(boost::any elem){ //Todo do pliku cpp
        	try{
				boost::any_cast<boost::none_t>(elem);
			}catch(boost::bad_any_cast &e){
				try{
					boost::any_cast<mtfifo::ExitThread>(elem);
					closeFlag_ = true;
				}catch(boost::bad_any_cast &e){
					assert(!"Unknown element type");
				}
			}
		}

		void operator()(){ //TODO Wyrzucic treœc do pliku .cpp
			receiverInit();
			mtfifo::FIFODistributor& fifoDistributor = mtfifo::FIFODistributor::getInstance();
			mtfifo::FIFO<mtfifo::FIFOInput> input
						= fifoDistributor.getFIFO<mtfifo::FIFOInput>(mtfifo::FIFO_DESERIALIZE);
			mtfifo::FIFO<mtfifo::FIFOOutput> output
						= fifoDistributor.getFIFO<mtfifo::FIFOOutput>(mtfifo::FIFO_DESERIALIZE);
			mtfifo::FIFO<mtfifo::FIFOOutput> log
									= fifoDistributor.getFIFO<mtfifo::FIFOOutput>(mtfifo::FIFO_LOG);

			boost::thread::id id = boost::this_thread::get_id();
			thd::ThreadRegistration& threadRegistration = thd::ThreadRegistration::getInstance();
			threadRegistration.registerThread(id, thd::TCPIP);

			boost::any logElem;
			boost::any outputElem;

			boost::random::mt19937 rng;
			boost::random::uniform_int_distribution<> ten(1,10);

			while(1){
		        if((realReceiverFd_ = accept(receiverFd_, (struct sockaddr*)&client_,
		        		&sizeSockaddrIn_)) < 0){
					logElem = mtfifo::LogElement(
							std::string("accept nie zwróci³o poprawnej wartoœci"),
							critical,
							boost::this_thread::get_id());
					log.put(logElem);
		        	continue;
		        }

		        while(1){
		        	processInput(input.get());
		        	if(closeFlag_)
		        		break;

					logElem = mtfifo::LogElement(
							std::string("Czekam na dane z socketa: ")
							+ boost::lexical_cast<std::string>(realReceiverFd_)
							+ std::string(" na porcie: ")
							+ boost::lexical_cast<std::string>(GAME_LISTEN_PORT),
							critical,
							boost::this_thread::get_id());
					log.put(logElem);
		        	recvResult_ = recv(realReceiverFd_, buffer_, BUFFER_SIZE, socketFlags_);
					if(recvResult_ == -1){
						logElem = mtfifo::LogElement(
								std::string("recv zwróci³o wartoœc ujemn¹"),
								critical,
								boost::this_thread::get_id());
						log.put(logElem);
						close(realReceiverFd_);
						break;
					}else if(recvResult_ == 0){
						logElem = mtfifo::LogElement(
								std::string("recv zwróci³o zakoñczenie po³¹czenia"),
								error,
								boost::this_thread::get_id());
						log.put(logElem);
						close(realReceiverFd_);
						break;
					}else{
						buffer_[recvResult_] = '\0';
						std::string buffer(buffer_);
						if(ten(rng) > 0){ //Zawsze prawdziwe
							logElem = mtfifo::LogElement(
									std::string("Odczytano dane: ") + buffer,
									normal,
									boost::this_thread::get_id());
							log.put(logElem);
						}
						outputElem = mtfifo::TCPIPSerialized(buffer);
					}
//					if(0){ //TODO jeœli jest co wys³ac, zrób to teraz
//						if (send(realReceiverFd, /*new*/buffer, strlen(buffer), flags) < 0){
//							 //TODO ERROR
//							 close(realReceiverFd);
//							 break;
//						}
//						//TODO send success
//					}
					boost::this_thread::sleep_for(TCPIP_RECEIVER_TIME);
		        }//while
				close(realReceiverFd_);
				if(closeFlag_)
					break;
				boost::this_thread::sleep_for(TCPIP_RECEIVER_TIME * 2);
			}//while
		}

		virtual ~TCPIPThreadReciever(){ //TODO wstawic do pliku cpp
		}

		private:
			bool closeFlag_;
			//Obs³uga socketów
			struct sockaddr_in receiverIn_;
			struct sockaddr_in client_;
			socklen_t sizeSockaddrIn_;
			int receiverFd_;
			int realReceiverFd_;
			int socketFlags_;
			int recvResult_;
			char buffer_[BUFFER_SIZE];
			char optval_;
	};
}

#endif /* GAME_INCLUDES_THREADS_TCPIP_TCPIPTHREADRECEIVER_HPP_ */
