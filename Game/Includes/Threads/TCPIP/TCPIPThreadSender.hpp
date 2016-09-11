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
#include <cstdlib>
#include <string>

//Boost
#include <boost/any.hpp>
#include <boost/none.hpp>
#include <boost/none_t.hpp>
#include <boost/lexical_cast.hpp>

//App
#include <GameAssert/GameAssert.hpp>

namespace thd{
	//Konfiguracja po³¹czenia wychodz¹cego
	const int GAME_SEND_PORT = 3097; //TODO wyrzucic do sk³adowej static
	const char* SERVER_ADDRESS = "127.0.0.1";
	const int MAX_ERROR_COUNTER = 15; //Arbitralnie dobrana wielkoœc
	const bool BLOCKING_MODE = true;
	//const int BUFFER_SIZE = 1024; //Ju¿ zdefiniowano w pliku: TCPIPThreadReceiver.hpp
	//int close(int s) {return closesocket(s);} //Ju¿ zdefiniowano w pliku: TCPIPThreadReceiver.hpp

	/**
	 * Klasa zawieraj¹ca obs³ugê wejœciowego stosu internetowego
	 */
	class TCPIPThreadSender{
	public:
		void senderInit(){//TODO do pliku cpp
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

		    if((host_ = gethostbyaddr((const char *)&ipv4addr_,
		    		sizeof(struct in_addr), AF_INET)) == NULL)
		    	gameAssert(!"gethostbyaddr failed");

		    if((socketFd_ = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		    	gameAssert(!"socket failed");

		    if(!setBlockingMode(socketFd_, BLOCKING_MODE))
		    	gameAssert("setBlockingMode failed, 2nd");

		    std::memset(&server_, 0, sizeof(server_));
		    server_.sin_family = AF_INET;
		    server_.sin_port = htons(GAME_SEND_PORT);
		    server_.sin_addr = *((struct in_addr *)host_->h_addr);

	    	closeFlag_ = false;
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
					gameAssert(!"Unknown element type");
				}
			}
		}

		void operator()(){ //TODO Wyrzucic treœc do pliku .cpp
			senderInit();
			mtfifo::FIFODistributor& fifoDistributor = mtfifo::FIFODistributor::getInstance();
			mtfifo::FIFO<mtfifo::FIFOInput> input
						= fifoDistributor.getFIFO<mtfifo::FIFOInput>(mtfifo::FIFO_TCPIP_SEN_EXIT);
			mtfifo::FIFO<mtfifo::FIFOInput> input2
						= fifoDistributor.getFIFO<mtfifo::FIFOInput>(mtfifo::FIFO_SERIALIZED_OUTPUT);
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

		    while(1){
		    	processInput(input.get());
				if(closeFlag_)
					break;

		    	if(connect(socketFd_, (struct sockaddr *)&server_, sizeof(struct sockaddr)) < 0){
		    		logMsg << notification;
		    		logMsg << "connect zwróci³o wartoœc ujemn¹";
		    		log << logMsg;
		    		boost::this_thread::sleep_for(TCPIP_SENDER_TIME);
		    		continue;
		    	}

		    	errorCounter_ = 0;

		    	//TODO tu prze³¹czyc w tryb blokuj¹cy
				while(1){
					processInput(input.get()); //Sprawdzenie czy wydano sygna³ zakoñczenia w¹tku
					if(closeFlag_)
						break;

					elem = input2.get();
					try{
						boost::any_cast<boost::none_t>(elem);
						boost::this_thread::sleep_for(TCPIP_SENDER_TIME);
						continue;
					}catch(boost::bad_any_cast &e){
						try{
							mtfifo::TCPIPSerialized msgToSend =
									boost::any_cast<mtfifo::TCPIPSerialized>(elem);
							//Gwarancja, ¿e jest null terminated
							const char *message = msgToSend.serialized.c_str();
							if((std::strlen(message) + 1) > BUFFER_SIZE)
								gameAssert(!"serialized message is too big");
							std::strcpy(buffer_, message);
						}catch(boost::bad_any_cast &e){
							gameAssert(!"Unknown element type");
						}
					}
					if((send(socketFd_, buffer_, strlen(buffer_), 0)) < 0){
						logMsg << critical;
						logMsg << "send zwróci³o wartoœc ujemn¹";
						log << logMsg;
						++errorCounter_;
						boost::this_thread::sleep_for(TCPIP_SENDER_TIME);
						if(errorCounter_ < MAX_ERROR_COUNTER){
							close(socketFd_);
							gameAssert(!"send failed"); //TODO To zakoñczy program w momencie roz³¹czenia
						}
					}else{
						errorCounter_ = 0;
						logMsg << normal;
						logMsg << "send powiod³o siê, wys³ano: " + std::string(buffer_);
						log << logMsg;
					}
					boost::this_thread::sleep_for(TCPIP_SENDER_TIME);
				}//while
				//TODO tu prze³¹czyc w tryb nieblokuj¹cy
				boost::this_thread::sleep_for(TCPIP_SENDER_TIME * 2);
		    }//while
			close(socketFd_);
		}

		virtual ~TCPIPThreadSender(){ //TODO wstawic do pliku cpp
		}

		private:
			bool closeFlag_;
			int errorCounter_;
			//Sockety
		    struct sockaddr_in server_;
		    struct hostent *host_;
		    int socketFd_;
		    char buffer_[BUFFER_SIZE];
		    struct in_addr ipv4addr_;
	};
}

#endif /* GAME_INCLUDES_THREADS_TCPIP_TCPIPTHREADSENDER_HPP_ */

