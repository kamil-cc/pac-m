/**
 * FIFOOutput.hpp
 *
 *  Created on: 27 sie 2016
 *      Author: Kamil Burzyñski
 */

#ifndef GAME_INCLUDES_MTFIFO_FIFOOUTPUT_HPP_
#define GAME_INCLUDES_MTFIFO_FIFOOUTPUT_HPP_

#include <MtFIFO/FIFOBase.hpp>
#include <iosfwd>
#include <cstring>

//App

namespace mtfifo{

class FIFOOutput : public FIFOBase{
public:
	FIFOOutput(){

	}

	virtual ~FIFOOutput(){

	}
};

}


#endif /* GAME_INCLUDES_MTFIFO_FIFOOUTPUT_HPP_ */
