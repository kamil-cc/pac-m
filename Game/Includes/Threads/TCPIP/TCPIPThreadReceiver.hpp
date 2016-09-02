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
		void operator()(){ //TODO Wyrzucic treœc do pliku .cpp
#ifdef __WIN32__
			WORD versionWanted = MAKEWORD(1, 1);
			WSADATA wsaData;
			WSAStartup(versionWanted, &wsaData);
#endif
			mtfifo::FIFODistributor& fifoDistributor = mtfifo::FIFODistributor::getInstance();
			mtfifo::FIFO<mtfifo::FIFOOutput> output
						= fifoDistributor.getFIFO<mtfifo::FIFOOutput>(mtfifo::FIFO_DESERIALIZE);
			mtfifo::FIFO<mtfifo::FIFOOutput> log
									= fifoDistributor.getFIFO<mtfifo::FIFOOutput>(mtfifo::FIFO_LOG);

			boost::thread::id id = boost::this_thread::get_id();
			thd::ThreadRegistration& threadRegistration = thd::ThreadRegistration::getInstance();
			threadRegistration.registerThread(id, thd::TCPIP);

			boost::random::mt19937 rng;
			boost::random::uniform_int_distribution<> ten(1,10);

			//Obs³uga socketów
			struct sockaddr_in receiverIn;
			std::memset(&receiverIn, 0, sizeof(receiverIn));
			struct sockaddr_in client;
			std::memset(&client, 0, sizeof(client));
			socklen_t size;
			int receiverFd;
			int realReceiverFd;
			int flags = 0; //??
			int recvResult;
			char buffer[BUFFER_SIZE];
		    std::memset(buffer, 0, sizeof(buffer)/sizeof(*buffer));
			char optval;

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

			while(1){
		        size = sizeof(struct sockaddr_in);

		        if((realReceiverFd = accept(receiverFd, (struct sockaddr*)&client, &size)) < 0){

		        	continue;
		        }

		        while(1){
		        	recvResult = recv(realReceiverFd, buffer, BUFFER_SIZE, flags);
					if(recvResult == -1){
						assert(!"1");
						//close(realReceiverFd); //??
						//TODO CRITICAL
					}else if(recvResult == 0){
						//TODO ERROR
						//close(realReceiverFd); //??
						break;
					}
					buffer[recvResult] = '\0';
					//TODO print to log and other queues
					if(0){ //TODO jeœli jest co wys³ac, zrób to teraz
						if (send(realReceiverFd, /*new*/buffer, strlen(buffer), flags) < 0){
							 //TODO ERROR
							 close(realReceiverFd);
							 break;
						}
						//TODO send success
					}
					boost::this_thread::sleep_for(TCPIP_RECEIVER_TIME);
		        }

				close(realReceiverFd);
				boost::this_thread::sleep_for(TCPIP_RECEIVER_TIME);
			}
		}

		virtual ~TCPIPThreadReciever(){ //TODO wstawic do pliku cpp
			//delete timeMaster; //Mo¿e byc niebezpieczne, jeœli timeMaster nie bêdzie zainicjalizowany
		}

		private:
			//TimeMaster *timeMaster;
			boost::chrono::milliseconds wait;
	};
}

#endif /* GAME_INCLUDES_THREADS_TCPIP_TCPIPTHREADRECEIVER_HPP_ */
