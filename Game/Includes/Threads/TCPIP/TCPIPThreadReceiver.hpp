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
	#include <fcntl.h>
	#include <sys/socket.h>
#endif

//Std
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <string>

//Boost
#include <boost/any.hpp>
#include <boost/none.hpp>
#include <boost/none_t.hpp>
#include <boost/lexical_cast.hpp>

//App
#include <MtFIFO/FIFODistributor.hpp>

namespace thd{
	//Konfiguracja po³¹czenia sieciowego
	const int GAME_LISTEN_PORT = 3098; //TODO wyrzucic do pliku cpp i zrobic ustawialne
	const int BUFFER_SIZE = 1024;

	/**
	 * Klasa zawieraj¹ca obs³ugê wejœciowego stosu internetowego
	 */
	class TCPIPThreadReceiver{
	public:
		void receiverInit(){//TODO do pliku cpp
#ifdef __WIN32__
			WORD versionWanted = MAKEWORD(1, 1);
			WSADATA wsaData;
			WSAStartup(versionWanted, &wsaData);
#endif
			std::memset(&receiverIn_, 0, sizeof(receiverIn_));
			std::memset(&client_, 0, sizeof(client_));
			std::memset(buffer_, 0, sizeof(buffer_));

			if((receiverFd_ = socket(AF_INET, SOCK_STREAM, 0)) < 0)
				assert(!"socked failed");

			if(setsockopt(receiverFd_, SOL_SOCKET, SO_REUSEADDR, &optval_, sizeof(optval_)) < 0)
				assert(!"setsockopt failed");

			//Ustawianie sockaddr_in
			receiverIn_.sin_family = AF_INET;
			receiverIn_.sin_port = htons(GAME_LISTEN_PORT);
			receiverIn_.sin_addr.s_addr = INADDR_ANY; //Przyjmuj wszystkie adresy.

			if(bind(receiverFd_, (struct sockaddr*)&receiverIn_, sizeof(struct sockaddr)) < 0)
				assert(!"bind failed");

			//http://tangentsoft.net/wskfaq/advanced.html#backlog
			if(listen(receiverFd_, SOMAXCONN) < 0)
				assert(!"listen failed");

			sizeSockaddrIn_ = sizeof(struct sockaddr_in);
			closeFlag_ = false;
			socketFlags_ = 0;

			if(!setBlockingMode(receiverFd_, true))
				assert(!"Failed to set nonblocking mode");
		}

		bool setBlockingMode(const int& socket, const bool blocking){ //TODO do pliku cpp
#ifdef __WIN32__
			unsigned long mode = blocking ? 0 : 1;
			if(ioctlsocket(socket, FIONBIO, &mode) == 0)
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
						= fifoDistributor.getFIFO<mtfifo::FIFOInput>(mtfifo::FIFO_TCPIP_REC_EXIT);
			mtfifo::FIFO<mtfifo::FIFOOutput> output
						= fifoDistributor.getFIFO<mtfifo::FIFOOutput>(mtfifo::FIFO_SERIALIZED_INPUT);
			mtfifo::FIFO<mtfifo::FIFOOutput> log
									= fifoDistributor.getFIFO<mtfifo::FIFOOutput>(mtfifo::FIFO_LOG);

			boost::thread::id id = boost::this_thread::get_id();
			thd::ThreadRegistration& threadRegistration = thd::ThreadRegistration::getInstance();
			threadRegistration.registerThread(id, thd::TCPIP_RECEIVER);

			boost::any outputElem;

			boost::random::mt19937 rng;
			boost::random::uniform_int_distribution<> ten(1,10);

			mtfifo::LogElement logMsg = mtfifo::LogElement();
			logMsg << id;

			while(1){
	        	processInput(input.get()); //Sprawdzenie czy wydano sygna³ zakoñczenia w¹tku
	        	if(closeFlag_)
	        		break;

				logMsg << normal;
				logMsg << "Czekam na wykonanie siê accept";
				log << boost::any(logMsg);

		        if((realReceiverFd_ = accept(receiverFd_, (struct sockaddr*)&client_,
		        		&sizeSockaddrIn_)) < 0){
		        	logMsg << notification;
					logMsg << "accept nie zwróci³o poprawnej wartoœci";
					log << boost::any(logMsg);
					boost::this_thread::sleep_for(TCPIP_RECEIVER_TIME);
		        	continue;
		        }

		        if(!setBlockingMode(realReceiverFd_, true))
		        	assert(!"Failed to set nonblocking mode, 2nd");

		        logMsg << normal;
				logMsg << "accept wykonane poprawnie. Zwrócono: "
						+ boost::lexical_cast<std::string>(realReceiverFd_);
				log << boost::any(logMsg);

		        while(1){
		        	processInput(input.get()); //Sprawdzenie czy wydano sygna³ zakoñczenia w¹tku
		        	if(closeFlag_)
		        		break;

		        	logMsg << normal;
					logMsg << "Czekam na dane z socketa: "
							+ boost::lexical_cast<std::string>(realReceiverFd_)
							+ " na porcie: " + boost::lexical_cast<std::string>(GAME_LISTEN_PORT);
					log << logMsg;

		        	recvResult_ = recv(realReceiverFd_, buffer_, BUFFER_SIZE, socketFlags_);
					if(recvResult_ <= -1){
						logMsg << critical;
						logMsg << "recv zwróci³o wartoœc ujemn¹: "
								+ boost::lexical_cast<std::string>(recvResult_);
						log << logMsg;
						//TODO
#ifdef __WIN32__
						logMsg << critical;
						logMsg << boost::lexical_cast<std::string>(WSAGetLastError());
						log << logMsg;
#endif
						//close(realReceiverFd_);
						//break;
					}else if(recvResult_ == 0){
						logMsg << notification;
						logMsg << "recv zwróci³o zakoñczenie po³¹czenia";
						log << logMsg;
						close(realReceiverFd_);
						break;
					}else{
						buffer_[recvResult_] = '\0';
						std::string buffer(buffer_);
						if(ten(rng) > 0){ //Zawsze prawdziwe
							logMsg << normal;
							logMsg << "Odczytano dane: " + buffer;
							log << logMsg;
						}
						outputElem = mtfifo::TCPIPSerialized(buffer);
					}
					boost::this_thread::sleep_for(TCPIP_RECEIVER_TIME);
		        }//while
				close(realReceiverFd_);
				if(closeFlag_)
					break;
				boost::this_thread::sleep_for(TCPIP_RECEIVER_TIME * 2);
			}//while
		}

		virtual ~TCPIPThreadReceiver(){ //TODO wstawic do pliku cpp
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
