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
	#include <winbase.h>
#else
	#include <cerrno>
	#include <fcntl.h>
	#include <sys/socket.h>
#endif

//Std
#include <cstdlib>
#include <iostream>
#include <string>

//Boost
#include <boost/algorithm/string.hpp>
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

namespace thd{
	/**
	 * Klasa zawieraj¹ca obs³ugê wejœciowego stosu internetowego
	 */
	class TCPIPThreadReceiver{
	public:
		void receiverInit();
		bool setBlockingMode(const int& socket, const bool blocking);
		void processInput(boost::any elem);
		void operator()();
		virtual ~TCPIPThreadReceiver();
	private:
		static const int GAME_LISTEN_PORT; //Konfiguracja portu po³¹czenia przychodz¹cego
	private:
#ifdef __WIN32__
		int close(int s) {return closesocket(s);}
#endif
	private:
		bool closeFlag_;
		bool blockingFlag_;
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
