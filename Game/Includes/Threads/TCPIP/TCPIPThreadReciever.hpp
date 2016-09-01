/*
 * TCPIPThread.hpp
 *
 *  Created on: 1 wrz 2016
 *      Author: Kamil Burzyñski
 */

#ifndef GAME_INCLUDES_THREADS_TCPIP_TCPIPTHREADRECIEVER_HPP_
#define GAME_INCLUDES_THREADS_TCPIP_TCPIPTHREADRECIEVER_HPP_

//Standardowe podejœcie do programowania uniwersalnych socketów
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
			//mtfifo::FIFO<mtfifo::FIFOInput> input
			//	= fifoDistributor.getFIFO<mtfifo::FIFOInput>(mtfifo::FIFO_TCP);
			mtfifo::FIFO<mtfifo::FIFOOutput> output
						= fifoDistributor.getFIFO<mtfifo::FIFOOutput>(mtfifo::FIFO_TCPIP);
			mtfifo::FIFO<mtfifo::FIFOOutput> log
									= fifoDistributor.getFIFO<mtfifo::FIFOOutput>(mtfifo::FIFO_LOG);

			boost::thread::id id = boost::this_thread::get_id();
			thd::ThreadRegistration& threadRegistration = thd::ThreadRegistration::getInstance();
			threadRegistration.registerThread(id, thd::TCPIP);

			//ThreadTime& threadTime = ThreadTime::getInstance();
			//timeMaster = threadTime.factory();

			boost::random::mt19937 rng;
			boost::random::uniform_int_distribution<> ten(1,10);

			//Sockety //TODO
			struct sockaddr_in receiverIn;
			std::memset(&receiverIn, 0, sizeof(receiverIn));
			struct sockaddr_in client;
			std::memset(&client, 0, sizeof(client));
			socklen_t size;
			int receiverFd;
			int realReceiverFd;
			int flags = 0;
			int recvResult; //TODO
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
				/*wait = timeMaster->calculateTime(LOG_TIME, input.size());
				if(ten(rng) == 1){ //Wiadomoœc debugowa. Œrednio co dziesi¹ta
					elem = mtfifo::LogElement(
							std::string("Czekam: ") + boost::lexical_cast<std::string>(wait)
							+ " Rozmiar: " + boost::lexical_cast<std::string>(input.size()),
							normal,
							boost::this_thread::get_id());
					output.put(elem);
				}*/

		        size = sizeof(struct sockaddr_in);

		        if((realReceiverFd = accept(receiverFd, (struct sockaddr*)&client, &size)) < 0)
		        	assert(!"accept failed");

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
		        }

				close(realReceiverFd);
		        //printf("Server got connection from client %s\n", inet_ntoa(dest.sin_addr));

		        /*while(1) {

				recvResult = recv(sender_fd, buffer, BUFFER_SIZE, flags);
                if(recvResult == -1)
                	;//assert(!"recv failed");
                else if(recvResult == 0)
                    assert(!"recv connection closed");
                buffer[recvResult] = '\0'; //Dodaje znak koñca wiadomoœci
                //TODO wys³anie wiadomoœci do loga*/
				//boost::this_thread::sleep_for(wait);
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

#endif /* GAME_INCLUDES_THREADS_TCPIP_TCPIPTHREADRECIEVER_HPP_ */
