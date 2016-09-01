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
	for (map<string, const FIFOImplementation*>::iterator it = fifoMap_.begin(),
			end = fifoMap_.end(); it != end; ++it){
	  delete it->second;
	}
	fifoMap_.clear();
}

const mtfifo::FIFOImplementation* mtfifo::FIFODistributor::getImplementation(const string& name){
	boost::lock_guard<FIFODistributor> guard(*this);
	if (fifoMap_.find(name) == fifoMap_.end()){ //Jeœli brak elementu, utwórz go
		fifoMap_.insert(pair<const string, const FIFOImplementation*>(string(name), new FIFOImplementation()));
	}
	//zwróc istniej¹cy element
	return fifoMap_[name];
}

/**
 * Pobieranie jedynej instancji klasy.
 * "in C++11 static locals are guaranteed to be initialized by the runtime in a thread-safe manner."
 * http://stackoverflow.com/questions/6116520/c-is-this-getinstance-method-of-my-singleton-threadsafe
 */
mtfifo::FIFODistributor& mtfifo::FIFODistributor::getInstance(){
	static FIFODistributor instance;
	return instance;
}

#endif /* GAME_MSGFIFO_MSGFIFO_HPP_ */
