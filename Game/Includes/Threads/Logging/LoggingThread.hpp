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
#ifdef PRINT_TYPEINFO
#include <typeinfo>
#endif

//Boost
#include <boost/any.hpp>
#include <boost/none.hpp>

//App
#include <MtFIFO/FIFO.hpp>
#include <MtFIFO/FIFODistributor.hpp>
#include <MtFIFO/FIFOInput.hpp>
#include <MtFIFO/Names.hpp>
#include <MtFIFO/Types.hpp>

//TODO usuñ
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
//#include <boost/log/>

namespace thd{

class LoggingThread{
public:
	void loggingInit(){//TODO Wyrzucic treœc do pliku .cpp
		boost::log::add_file_log("sample.log");
	}

	void operator()(){ //TODO Wyrzucic treœc do pliku .cpp
		loggingInit();
		mtfifo::FIFODistributor& fifoDistributor = mtfifo::FIFODistributor::getInstance();
		mtfifo::FIFO<mtfifo::FIFOInput> input = fifoDistributor.getFIFO<mtfifo::FIFOInput>(mtfifo::FIFO_LOG);

		while(1){
			boost::any elem = input.get();
			try{
				mtfifo::LogElement logElement = boost::any_cast<mtfifo::LogElement>(elem);
				//boost::log::v2s_mt_nt5::trivial::severity_level lvl = logElement.level;
				boost::log::sources::severity_logger< boost::log::trivial::severity_level > lg;
				BOOST_LOG_SEV(lg,logElement.level) << logElement.value;
			}catch (boost::bad_any_cast &e){
				try{
					boost::any_cast<boost::none_t>(elem);
				}catch (boost::bad_any_cast &e){
					try{
						boost::any_cast<mtfifo::ExitThread>(elem);
						break;
					}catch(boost::bad_any_cast &e){
						assert(!"Unknown element type");
					}
				}
			}
			boost::this_thread::sleep_for(boost::chrono::seconds(1));
		}
	}
};

}



#endif /* GAME_INCLUDES_THREADS_LOGGING_LOGGINGTHREAD_HPP_ */
