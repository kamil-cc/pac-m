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

/*
 * Pobieranie jedynej instancji klasy
 */
mtfifo::FIFODistributor& mtfifo::FIFODistributor::getInstance(){ //TODO potencjalne niebezpieczeñstwo dostêpu wspó³bie¿nego
	//TODO za³o¿yc jakiœ mutex na funkcjê static
	static FIFODistributor instance;
	return instance;
}

#endif /* GAME_MSGFIFO_MSGFIFO_HPP_ */
