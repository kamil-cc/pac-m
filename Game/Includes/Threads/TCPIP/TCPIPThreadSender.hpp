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
#include <boost/random.hpp>
#include <boost/thread.hpp>

//App
#include <GameAssert/GameAssert.hpp>
#include <MtFIFO/FIFODistributor.hpp>
#include <MtFIFO/Names.hpp>
#include <MtFIFO/Types.hpp>
#include <Threads/TCPIP/TCPIPInternalConfig.hpp>
#include <Threads/ThreadNames.hpp>
#include <Threads/ThreadRegistration.hpp>
#include <Threads/ThreadTime.hpp>
#include <Threads/TCPIP/TCPIPInternalConfig.hpp>

namespace thd{
	/**
	 * Klasa zawieraj¹ca obs³ugê wejœciowego stosu internetowego
	 */
	class TCPIPThreadSender{
	public:
		void senderInit();
		bool setBlockingMode(const int& socket, const bool blocking);
		void processInput(boost::any elem);
		void operator()();
		virtual ~TCPIPThreadSender();
	private:
		//Konfiguracja po³¹czenia wychodz¹cego
		static const int GAME_SEND_PORT;
		const char* SERVER_ADDRESS;
		//Konfiguracja socketów
		static const int MAX_ERROR_COUNTER;
		static const bool BLOCKING_MODE;
	private:
		int close(int s) {return closesocket(s);}
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

