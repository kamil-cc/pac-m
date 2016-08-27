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

//sem_t FIFODistributor::semaphore; //TODO
//map<string, FIFOPtr> FIFODistributor::fifos;

FIFODistributor::FIFODistributor() {
	//Inicjalizacja semafora
	//sem_init(&semaphore, 0, 1); //TODO
}

FIFODistributor::~FIFODistributor() {
	//Zwolnienie w¹tków oczekuj¹cych na semafor
	int value;
	do {
		//sem_getvalue(&semaphore, &value); //TODO
		if (value == 0) {
			//sem_post(&semaphore); //TODO
			BOOST_LOG_TRIVIAL(trace)<< "Remove waiting threads from semaphore, ~MsgFIFODistributor";
		}
	}while(value != 1);

	//Próba destrukcji semafora
	//TODO
	/*if(sem_destroy(&semaphore) < 0){
		BOOST_LOG_TRIVIAL(warning)<< "sem_destroy() failed, ~MsgFIFODistributor";
	}*/
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
