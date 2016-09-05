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

//Boost
#include <boost/any.hpp>
#include <boost/chrono.hpp>
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
		//mtfifo::FIFO<mtfifo::FIFOOutput> output
		//	= fifoDistributor.getFIFO<mtfifo::FIFOOutput>(mtfifo::FIFO_LOG);
		mtfifo::FIFO<mtfifo::FIFOOutput> output
					= fifoDistributor.getFIFO<mtfifo::FIFOOutput>(mtfifo::FIFO_SERIALIZED_OUTPUT);

		boost::thread::id id = boost::this_thread::get_id();
		thd::ThreadRegistration& threadRegistration = thd::ThreadRegistration::getInstance();
		threadRegistration.registerThread(id, thd::FAKE);

		for(int i = 0; i < 1000; ++i){
			std::string str = "Wiadomosc nr: " + boost::lexical_cast<std::string>(i);
			//boost::any elem = mtfifo::LogElement(str, normal, boost::this_thread::get_id());
			boost::any elem = mtfifo::TCPIPSerialized(str);
			output.put(elem);

			boost::this_thread::sleep_for(boost::chrono::milliseconds(50));
		}
	}
};

}

#endif /* GAME_INCLUDES_THREADS_FAKESENDER_FAKESENDER_HPP_ */
