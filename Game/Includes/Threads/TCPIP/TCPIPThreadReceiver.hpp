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

			//Obs³uga socketów
			struct sockaddr_in receiverIn;
			struct sockaddr_in client;
			socklen_t sizeSockaddrIn;
			int receiverFd;
			int realReceiverFd;
			int socketFlags;
			int recvResult;
			char buffer[BUFFER_SIZE];
			char optval;

			std::memset(&receiverIn, 0, sizeof(receiverIn));
			std::memset(&client, 0, sizeof(client));
		    std::memset(buffer, 0, sizeof(buffer)/sizeof(*buffer));

			if((receiverFd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
				assert(!"socked failed");

			if(setsockopt(receiverFd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0)
				assert(!"setsockopt failed");

			//Ustawianie sockaddr_in
			receiverIn.sin_family = AF_INET;
			receiverIn.sin_port = htons(GAME_LISTEN_PORT);
			receiverIn.sin_addr.s_addr = INADDR_ANY;

			if(bind(receiverFd, (struct sockaddr*)&receiverIn, sizeof(struct sockaddr)) < 0)
				assert(!"bind failed");

			//http://tangentsoft.net/wskfaq/advanced.html#backlog
			if(listen(receiverFd, SOMAXCONN) < 0)
				assert(!"listen failed");

			sizeSockaddrIn = sizeof(struct sockaddr_in);
			closeFlag_ = false;
			socketFlags = 0;

			while(1){
		        if((realReceiverFd = accept(receiverFd, (struct sockaddr*)&client,
		        		&sizeSockaddrIn)) < 0){
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

		        	recvResult = recv(realReceiverFd, buffer, BUFFER_SIZE, socketFlags);
					if(recvResult == -1){
						logElem = mtfifo::LogElement(
								std::string("recv zwróci³o wartoœc ujemn¹"),
								critical,
								boost::this_thread::get_id());
						log.put(logElem);
						close(realReceiverFd);
						break;
					}else if(recvResult == 0){
						logElem = mtfifo::LogElement(
								std::string("recv zwróci³o zakoñczenie po³¹czenia"),
								error,
								boost::this_thread::get_id());
						log.put(logElem);
						close(realReceiverFd);
						break;
					}else{
						buffer[recvResult] = '\0';
						if(ten(rng) > 0){ //Zawsze prawdziwe
							logElem = mtfifo::LogElement(
									std::string("Odczytano dane: ") + std::string(buffer),
									normal,
									boost::this_thread::get_id());
							log.put(logElem);
						}
						outputElem = mtfifo::LogElement(
								std::string("Odczytano dane: ") + std::string(buffer),
								normal,
								boost::this_thread::get_id());
						output.put(outputElem);
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
				close(realReceiverFd);
				if(closeFlag_)
					break;
				boost::this_thread::sleep_for(TCPIP_RECEIVER_TIME * 2);
			}//while
		}

		virtual ~TCPIPThreadReciever(){ //TODO wstawic do pliku cpp
		}

		private:
			bool closeFlag_;
	};
}

#endif /* GAME_INCLUDES_THREADS_TCPIP_TCPIPTHREADRECEIVER_HPP_ */
