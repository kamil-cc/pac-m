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
#include <boost/filesystem.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/path_traits.hpp>
#include <boost/filesystem/config.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/filesystem/exception.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/detail/utf8_codecvt_facet.hpp>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup.hpp>
#include <boost/none.hpp>

//App
#include <MtFIFO/FIFO.hpp>
#include <MtFIFO/FIFODistributor.hpp>
#include <MtFIFO/FIFOInput.hpp>
#include <MtFIFO/Names.hpp>
#include <MtFIFO/Types.hpp>

using namespace std;

namespace thd{

class LoggingThread{
public:
	void loggingInit(){//TODO Wyrzucic treœc do pliku .cpp
		//string ssi = "ALC";
		//boost::log::add_file_log(boost::log::keywords::file_name = "xxy.log");
		//boost::log::core::get()->set_filter(boost::log::trivial::severity
		//		>= boost::log::trivial::info);
	}

	void operator()(){ //TODO Wyrzucic treœc do pliku .cpp
		loggingInit();
		mtfifo::FIFODistributor& fifoDistributor = mtfifo::FIFODistributor::getInstance();
		mtfifo::FIFO<mtfifo::FIFOInput> input = fifoDistributor.getFIFO<mtfifo::FIFOInput>(mtfifo::FIFO_LOG);

		while(1){
			boost::any elem = input.get();
			try{
				mtfifo::LogElement logElement = boost::any_cast<mtfifo::LogElement>(elem);
				//BOOST_LOG_TRIVIAL(static_cast<info>(boost::log::v2s_mt_nt5::trivial)) << logElement.value; //TODO
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
