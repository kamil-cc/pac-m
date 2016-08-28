/*
 * LoggingThread.hpp
 *
 *  Created on: 28 sie 2016
 *      Author: Kamil Burzyñski
 */

#ifndef GAME_INCLUDES_THREADS_LOGGING_LOGGINGTHREAD_HPP_
#define GAME_INCLUDES_THREADS_LOGGING_LOGGINGTHREAD_HPP_

//Tmp
#include <iostream>

//Std
#include <string>

//Boost
#include <boost/any.hpp>
#include <boost/none.hpp>

//App
#include <MtFIFO/FIFO.hpp>
#include <MtFIFO/FIFODistributor.hpp>
#include <MtFIFO/FIFOInput.hpp>
#include <MtFIFO/Names.hpp>
#include <MtFIFO/Types.hpp>

namespace thd{

class LoggingThread{
public:
	void operator()(){ //TODO Wyrzucic treœc do pliku .cpp
		mtfifo::FIFODistributor& fifoDistributor = mtfifo::FIFODistributor::getInstance();
		mtfifo::FIFO<mtfifo::FIFOInput> input = fifoDistributor.getFIFO<mtfifo::FIFOInput>(mtfifo::FIFO_LOG);

		for(int i = 0; i < 100; ++i){
			boost::any elem = input.get();
			try{
				mtfifo::StringElement stringElement = boost::any_cast<mtfifo::StringElement>(elem);
				std::cout << stringElement.value << std::endl;
			}catch (boost::bad_any_cast &e){
				try{
					boost::any_cast<boost::none_t>(elem);
				}catch (boost::bad_any_cast &e){
					assert(!"Unknown element type");
				}
			}
			boost::this_thread::sleep_for(boost::chrono::seconds(1));
		}
	}
};

}



#endif /* GAME_INCLUDES_THREADS_LOGGING_LOGGINGTHREAD_HPP_ */
