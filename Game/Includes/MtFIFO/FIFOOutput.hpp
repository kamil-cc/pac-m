/**
 * FIFOOutput.hpp
 *
 *  Created on: 27 sie 2016
 *      Author: Kamil Burzyñski
 */

#ifndef GAME_INCLUDES_MTFIFO_FIFOOUTPUT_HPP_
#define GAME_INCLUDES_MTFIFO_FIFOOUTPUT_HPP_

//Std
#include <string>

//Boost
#include <boost/any.hpp>

//App
#include <CommonTypes.hpp>
#include <MtFIFO/FIFODistributor.hpp>
#include <MtFIFO/FIFOImplementation.hpp>

namespace mtfifo{

class FIFOOutput{
public:
	FIFOOutput(const std::string& name);
	virtual ~FIFOOutput();
	fifo_t get();
	void put(fifo_t elem);
	size_t size();
private:
	FIFOImplementation* impl_;
};

}


#endif /* GAME_INCLUDES_MTFIFO_FIFOOUTPUT_HPP_ */
