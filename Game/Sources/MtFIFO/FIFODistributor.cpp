/**
 * FIFODistributor.cpp
 *
 *  Created on: 27 sie 2016
 *      Author: Kamil Burzyñski
 */

#ifndef GAME_MSGFIFO_MSGFIFO_HPP_
#define GAME_MSGFIFO_MSGFIFO_HPP_

#include <MtFIFO/FIFODistributor.hpp>

mtfifo::FIFODistributor::FIFODistributor(){
}

mtfifo::FIFODistributor::~FIFODistributor(){
	//TODO for all in map free memory and remove from map's
}

mtfifo::FIFOImplementation* mtfifo::FIFODistributor::getImplementation(const string& name){
	boost::lock_guard<FIFODistributor> guard(*this);
	if (fifoMap_.find(name) == fifoMap_.end()){
		fifoMap_.insert(pair<string, FIFOImplementation*>(name, new FIFOImplementation()));
	}
	//zwróc istniej¹cy element
	return NULL;
}

/*
 * Pobieranie jedynej instancji klasy
 */
mtfifo::FIFODistributor& mtfifo::FIFODistributor::getInstance(){
	static FIFODistributor instance;
	return instance;
}

#endif /* GAME_MSGFIFO_MSGFIFO_HPP_ */
