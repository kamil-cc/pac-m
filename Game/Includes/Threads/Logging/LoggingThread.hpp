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

//App
#include <MtFIFO/FIFO.hpp>
#include <MtFIFO/FIFODistributor.hpp>
#include <MtFIFO/FIFOInput.hpp>
#include <MtFIFO/Names.hpp>
#include <MtFIFO/Types.hpp>
#include <Threads/ThreadRegistration.hpp>
#include <Threads/ThreadNames.hpp>
#include <Threads/ThreadTime.hpp>

//Operator wyœwietlaj¹cy poziom loga w zrozumia³ej dla cz³owieka postaci
std::ostream& operator<<(std::ostream& strm, severity_log_level level){
    static const char* strings[] = {
        "[NORMAL]",
        "[NOTIFICATION]",
        "[WARNING]",
        "[ERROR]",
        "[CRITICAL]"
    };

    if (static_cast<std::size_t>(level) < (sizeof(strings) / sizeof(*strings)))
        strm << strings[level];
    else
        strm << static_cast<int>(level);

    return strm;
}

namespace thd{

class LoggingThread{
public:
	void loggingInit(){//TODO Wyrzucic treœc do pliku .cpp
		boost::log::register_simple_formatter_factory<severity_log_level, char>("Severity");
		//W tym miejscu mo¿na regulowac poziom intensywnoœci loga.
		boost::log::core::get()->set_filter(
				boost::log::expressions::attr<severity_log_level>("Severity") >= normal);
		//Log do pliku
		boost::log::add_file_log(
				boost::log::keywords::file_name  = "game.log",
				boost::log::keywords::format = "%Severity% %Message%",
				boost::log::keywords::auto_flush = true);
		//Log na konsolê
		boost::log::add_console_log(
				std::cout,
				boost::log::keywords::format = "%Severity% %Message%",
				boost::log::keywords::auto_flush = true);
	}

	void operator()(){ //TODO Wyrzucic treœc do pliku .cpp
		loggingInit();
		boost::log::sources::severity_logger<severity_log_level> log;

		mtfifo::FIFODistributor& fifoDistributor = mtfifo::FIFODistributor::getInstance();
		mtfifo::FIFO<mtfifo::FIFOInput> input
			= fifoDistributor.getFIFO<mtfifo::FIFOInput>(mtfifo::FIFO_LOG);
		mtfifo::FIFO<mtfifo::FIFOOutput> output
					= fifoDistributor.getFIFO<mtfifo::FIFOOutput>(mtfifo::FIFO_LOG);

		boost::thread::id id = boost::this_thread::get_id();
		thd::ThreadRegistration& threadRegistration = thd::ThreadRegistration::getInstance();
		threadRegistration.registerThread(id, thd::LOGGER);

		ThreadTime& threadTime = ThreadTime::getInstance();
		timeMasterPtr_ = threadTime.factory();


		boost::random::mt19937 rng;
		boost::random::uniform_int_distribution<> ten(1,10);

		while(1){
			boost::any elem = input.get();
			try{
				mtfifo::LogElement logElement = boost::any_cast<mtfifo::LogElement>(elem);
				BOOST_LOG_SEV(log, logElement.level)
					<< "[" << threadRegistration.getName(logElement.id) << "] " << logElement.value;
			}catch(boost::bad_any_cast &e){
				try{
					boost::any_cast<boost::none_t>(elem);
				}catch(boost::bad_any_cast &e){
					try{
						boost::any_cast<mtfifo::ExitThread>(elem);
						break;
					}catch(boost::bad_any_cast &e){
						assert(!"Unknown element type");
					}
				}
			}
			wait_ = timeMasterPtr_->calculateTime(LOG_TIME, input.size());
			if(ten(rng) == 1){ //Wiadomoœc debugowa. Œrednio co dziesi¹ta
				elem = mtfifo::LogElement(
						std::string("Czekam: ") + boost::lexical_cast<std::string>(wait_)
						+ " Rozmiar: " + boost::lexical_cast<std::string>(input.size()),
						normal,
						boost::this_thread::get_id());
				output.put(elem);
			}
			boost::this_thread::sleep_for(wait_);
		}
	}

	virtual ~LoggingThread(){ //TODO wstawic do pliku cpp
		delete timeMasterPtr_; //Mo¿e byc niebezpieczne, jeœli timeMaster nie bêdzie zainicjalizowany
	}

	private:
		TimeMaster *timeMasterPtr_;
		boost::chrono::milliseconds wait_;
};

}



#endif /* GAME_INCLUDES_THREADS_LOGGING_LOGGINGTHREAD_HPP_ */
