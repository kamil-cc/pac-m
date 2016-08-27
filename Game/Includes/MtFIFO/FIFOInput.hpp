/**
 * FIFOInput.hpp
 *
 *  Created on: 27 sie 2016
 *      Author: Kamil Burzyñski
 */

#ifndef GAME_INCLUDES_MTFIFO_FIFOINPUT_HPP_
#define GAME_INCLUDES_MTFIFO_FIFOINPUT_HPP_

//Standard
#include <MtFIFO/FIFOBase.hpp>
#include <MtFIFO/FIFOIOSeparator.hpp>
#include <iostream>
#include <cstring>
#include <sys/types.h>
//#include <semaphore.h>
#include <queue>

//App

using namespace std;

namespace mtfifo{

class FIFOInput : public FIFOBase{
public:
	FIFOInput(){
		//FifoIOSeparator& fifoIOSeparator = FifoIOSeparator::getInstance();
		//fifoIOSeparator.getFIFO(this, name);
	}

	virtual ~FIFOInput(){

	}
private:
	//sem_t& semaphore;
	//queue<T>* dataFIFO;*/
};

}

#endif /* GAME_INCLUDES_MTFIFO_FIFOINPUT_HPP_ */
