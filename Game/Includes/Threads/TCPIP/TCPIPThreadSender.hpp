/**
 * TCPIPThreadSender.hpp
 *
 *  Created on: 2 wrz 2016
 *      Author: Kamil Burzyñski
 */

#ifndef GAME_INCLUDES_THREADS_TCPIP_TCPIPTHREADSENDER_HPP_
#define GAME_INCLUDES_THREADS_TCPIP_TCPIPTHREADSENDER_HPP_

//Przenoœne podejœcie do socketów Windows/Linux
#ifdef __WIN32__
	#include <winsock2.h>
	#include <ws2tcpip.h>
extern "C" {
	#include <inet_pton.h>
}
	//int close(int s) {return closesocket(s);} //Ju¿ zdefiniowano
#else
	#include <arpa/inet.h>
	#include <fcntl.h>
	#include <sys/socket.h>
#endif

//Std
#include <cassert>
#include <cstdlib>
#include <string>

//Boost
#include <boost/any.hpp>
#include <boost/none.hpp>
#include <boost/none_t.hpp>
#include <boost/lexical_cast.hpp>

namespace thd{
	//Konfiguracja po³¹czenia sieciowego
	const int GAME_SEND_PORT = 3098; //TODO wyrzucic do sk³adowej static
	const char* SERVER_ADDRESS= "127.0.0.1";
	const int MAX_ERROR_COUNTER = 15; //Arbitralnie dobrana wielkoœc
	//const int BUFFER_SIZE = 1024; //Ju¿ zdefiniowano

	/**
	 * Klasa zawieraj¹ca obs³ugê wejœciowego stosu internetowego
	 */
	class TCPIPThreadSender{
	public:
		void receiverInit(){//TODO do pliku cpp
#ifdef __WIN32__
			WORD versionWanted = MAKEWORD(1, 1);
			WSADATA wsaData;
			WSAStartup(versionWanted, &wsaData);
#endif
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
						= fifoDistributor.getFIFO<mtfifo::FIFOInput>(mtfifo::FIFO_TCPIP_SEN_EXIT);
			mtfifo::FIFO<mtfifo::FIFOInput> input2
						= fifoDistributor.getFIFO<mtfifo::FIFOInput>(mtfifo::FIFO_SERIALIZED_INPUT);
			mtfifo::FIFO<mtfifo::FIFOOutput> log
									= fifoDistributor.getFIFO<mtfifo::FIFOOutput>(mtfifo::FIFO_LOG);

			boost::thread::id id = boost::this_thread::get_id();
			thd::ThreadRegistration& threadRegistration = thd::ThreadRegistration::getInstance();
			threadRegistration.registerThread(id, thd::TCPIP_SENDER);

			boost::any elem;

			boost::random::mt19937 rng;
			boost::random::uniform_int_distribution<> ten(1,10);

			mtfifo::LogElement logMsg = mtfifo::LogElement();
			logMsg << id;

		    struct sockaddr_in server;
		    struct hostent *host;
		    int socketFd, num;
		    char buffer[BUFFER_SIZE];

		    char buff[BUFFER_SIZE];

		    struct in_addr ipv4addr;
		    std::memset(&ipv4addr, 0, sizeof(ipv4addr));
		    /*if((host = gethostbyname("example.com")) == NULL){
				assert(!"gethostbyname failed");
		    }*/

		    inet_pton(AF_INET, SERVER_ADDRESS, &ipv4addr);

		    if((host = gethostbyaddr((const char *)&ipv4addr,
		    		sizeof(struct in_addr), AF_INET)) == NULL)
		    	assert(!"gethostbyaddr failed");

		    logMsg << notification;
		    logMsg << "gethostbyaddr returned: " + std::string(*((struct in_addr *)host->h_addr));
		    log << logMsg;


		    if((socketFd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		    	assert(!"socket failed");

		    if(!setBlockingMode(socketFd, false))
		    	assert("setBlockingMode failed, 2nd");

		    logMsg << normal;
		    logMsg << "setting nonblocking mode, 2nd";
		    log << logMsg;

		    std::memset(&server, 0, sizeof(server));
		    server.sin_family = AF_INET;
		    server.sin_port = htons(GAME_SEND_PORT);
		    server.sin_addr = *((struct in_addr *)host->h_addr);
		    if(connect(socketFd, (struct sockaddr *)&server, sizeof(struct sockaddr)) < 0)
		        assert(!"connect failed");

		    errorCounter = 0;

			while(1){
	        	processInput(input.get()); //Sprawdzenie czy wydano sygna³ zakoñczenia w¹tku
	        	if(closeFlag_)
	        		break;
				//buffer = "some message"; //TODO usun¹c
				elem = input2.get();
				try{
					boost::any_cast<boost::none_t>(elem);
					boost::this_thread::sleep_for(TCPIP_SENDER_TIME);
					continue;
				}catch(boost::bad_any_cast &e){
					try{
						mtfifo::TCPIPSerialized msgToSend = boost::any_cast<mtfifo::TCPIPSerialized>(elem);
						const char *message = msgToSend.serialized.c_str();
						std::strncpy(buffer, message, sizeof(message));
					}catch(boost::bad_any_cast &e){
						assert(!"Unknown element type");
					}
				}
		        if((send(socketFd, buffer, strlen(buffer), 0)) < 0){
					logMsg << critical;
					logMsg << "send failed";
					log << logMsg;
					++errorCounter;
					if(errorCounter < MAX_ERROR_COUNTER){
						close(socketFd);
						assert(!"send failed");
					}
		        }else{
		        	errorCounter = 0;
					logMsg << normal;
					logMsg << "send ok: " + std::string();
					log << logMsg;
		        }
				boost::this_thread::sleep_for(TCPIP_SENDER_TIME);
			}//while
			close(socketFd);
		}

		virtual ~TCPIPThreadSender(){ //TODO wstawic do pliku cpp
		}

		private:
			bool closeFlag_;
			int errorCounter;
			/*//Obs³uga socketów
			struct sockaddr_in receiverIn_;
			struct sockaddr_in client_;
			socklen_t sizeSockaddrIn_;
			int receiverFd_;
			int realReceiverFd_;
			int socketFlags_;
			int recvResult_;
			char buffer_[BUFFER_SIZE];
			char optval_;*/
	};
}

#endif /* GAME_INCLUDES_THREADS_TCPIP_TCPIPTHREADSENDER_HPP_ */

