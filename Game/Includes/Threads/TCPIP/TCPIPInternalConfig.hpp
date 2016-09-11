/*
 * TCPIPInternalConfig.hpp
 *
 *  Created on: 11 wrz 2016
 *      Author: Kamil Burzyñski
 */

#ifndef GAME_INCLUDES_THREADS_TCPIP_TCPIPINTERNALCONFIG_HPP_
#define GAME_INCLUDES_THREADS_TCPIP_TCPIPINTERNALCONFIG_HPP_

#ifdef __WIN32__
	#include <winsock2.h>
#else
	#include <sys/socket.h>
#endif

namespace thd{
	const int BUFFER_SIZE = 1024;
	const int SOCK_TYPE = SOCK_STREAM; //lub SOCK_DGRAM
}

#endif /* GAME_INCLUDES_THREADS_TCPIP_TCPIPINTERNALCONFIG_HPP_ */
