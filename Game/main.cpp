/**
 * main.cpp
 *
 *  Created on: 28 sie 2016
 *      Author: Kamil Burzyñski
 */
//Boost
#include <boost/thread.hpp>

//App
#include <Threads/Logging/LoggingThread.hpp>
#include <Threads/FakeSender/FakeSender.hpp>
#include <Threads/TCPIP/TCPIPThreadReceiver.hpp>
#include <Threads/TCPIP/TCPIPThreadSender.hpp>
#include <Threads/Engine/ThreadGameEngine.hpp>

using namespace thd;

//Start aplikacji
int main() {
	LoggingThread log;
	boost::thread loggingThread(log);

	FakeSender send;
	boost::thread fakeSender(send);

	FakeSender send2;
	boost::thread fakeSender2(send2);

	TCPIPThreadReceiver TCPIPStackReceiver;
	boost::thread receiverThread(TCPIPStackReceiver);

	TCPIPThreadSender TCPIPStackSender;
	boost::thread senderThread(TCPIPStackSender);

	GameEngine gameEngine;
	boost::thread gameEngineThread(gameEngine);

	//fakeSender.join();
	//fakeSender2.join();
	loggingThread.join();
	receiverThread.join();
	senderThread.join();

	return 0;
}
