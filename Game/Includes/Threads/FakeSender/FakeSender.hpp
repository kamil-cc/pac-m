/**
 * FakeSender.hpp
 *
 *  Created on: 28 sie 2016
 *      Author: Kamil Burzyñski
 */

#ifndef GAME_INCLUDES_THREADS_FAKESENDER_FAKESENDER_HPP_
#define GAME_INCLUDES_THREADS_FAKESENDER_FAKESENDER_HPP_

//Std
#include <string>
#include <sstream>

//Boost
#include <boost/any.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/log/trivial.hpp>
#include <boost/none.hpp>

//App
#include <MtFIFO/FIFO.hpp>
#include <MtFIFO/FIFODistributor.hpp>
#include <MtFIFO/FIFOInput.hpp>
#include <MtFIFO/Names.hpp>
#include <MtFIFO/Types.hpp>

namespace thd{

class FakeSender{
public:
	void operator()(){
		mtfifo::FIFODistributor& fifoDistributor = mtfifo::FIFODistributor::getInstance();
		mtfifo::FIFO<mtfifo::FIFOOutput> output = fifoDistributor.getFIFO<mtfifo::FIFOOutput>(mtfifo::FIFO_LOG);

		for(int i = 0; i < 100; ++i){
			std::string str = "Wiadomosc nr: " + boost::lexical_cast<std::string>(i);
			boost::any elem = mtfifo::LogElement(str, boost::log::trivial::severity_level::info);
			output.put(elem);
			boost::this_thread::sleep_for(boost::chrono::seconds(3));
		}
	}
};

}

#endif /* GAME_INCLUDES_THREADS_FAKESENDER_FAKESENDER_HPP_ */
