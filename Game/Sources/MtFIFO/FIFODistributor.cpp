/**
 * FIFODistributor.cpp
 *
 *  Created on: 27 sie 2016
 *      Author: Kamil Burzyñski
 */

#ifndef GAME_MSGFIFO_MSGFIFO_HPP_
#define GAME_MSGFIFO_MSGFIFO_HPP_

#include <MtFIFO/FIFODistributor.hpp>

namespace mtfifo {

FIFODistributor::FIFODistributor() {
}

FIFODistributor::~FIFODistributor() {
}

/*
 * Pobieranie jedynej instancji klasy
 */
FIFODistributor& FIFODistributor::getInstance(){
	static FIFODistributor instance;
	return instance;
}

}

#endif /* GAME_MSGFIFO_MSGFIFO_HPP_ */
