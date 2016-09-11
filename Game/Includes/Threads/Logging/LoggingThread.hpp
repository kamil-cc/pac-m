/**
 * LoggingThread.hpp
 *
 *  Created on: 28 sie 2016
 *      Author: Kamil Burzyñski
 */

#ifndef GAME_INCLUDES_THREADS_LOGGING_LOGGINGTHREAD_HPP_
#define GAME_INCLUDES_THREADS_LOGGING_LOGGINGTHREAD_HPP_

//Std
#include <string>
#include <iostream>

//Boost
#include <boost/any.hpp>
#include <boost/chrono.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/none.hpp>
#include <boost/random.hpp>
#include <boost/thread.hpp>

//Ncurses
#include <ncursesw/ncurses.h>

//App
#include <GameAssert/GameAssert.hpp>
#include <MtFIFO/FIFO.hpp>
#include <MtFIFO/FIFODistributor.hpp>
#include <MtFIFO/FIFOInput.hpp>
#include <MtFIFO/Names.hpp>
#include <MtFIFO/Types.hpp>
#include <Threads/ThreadRegistration.hpp>
#include <Threads/ThreadNames.hpp>
#include <Threads/ThreadTime.hpp>

namespace thd{

class LoggingThread{
public:
	void loggingInit();
	void operator()();
	virtual ~LoggingThread();
private:
	TimeMaster *timeMasterPtr_;
	boost::chrono::milliseconds wait_;
};

}

#endif /* GAME_INCLUDES_THREADS_LOGGING_LOGGINGTHREAD_HPP_ */
