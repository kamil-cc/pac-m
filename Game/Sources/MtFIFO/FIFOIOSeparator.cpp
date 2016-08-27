/**
 * FIFOIOSeparator.cpp
 *
 *  Created on: 27 sie 2016
 *      Author: Kamil Burzyñski
 */

#include <MtFIFO/FIFOIOSeparator.hpp>

namespace mtfifo {

//sem_t FifoIOSeparator::semaphore; //TODO
//map<string, SemaphorePtr> FifoIOSeparator::FIFOSemaphores; //TODO
map<string, UniversalFIFOPtr> FIFOIOSeparator::FIFOStorage;

FIFOIOSeparator& FIFOIOSeparator::getInstance() {
	static FIFOIOSeparator instance;
	return instance;
}

FIFOIOSeparator::FIFOIOSeparator() {
	//Inicjalizacja semafora
	//sem_init(&semaphore, 0, 1); //TODO
}

FIFOIOSeparator::~FIFOIOSeparator() {
//Zwolnienie w¹tków oczekuj¹cych na semafor
	int value;
	do {
		//sem_getvalue(&semaphore, &value); //TODO
		value = 0; //TODO
		if (value == 0) {
			//sem_post(&semaphore); //TODO
			BOOST_LOG_TRIVIAL(trace) << "Remove waiting threads from semaphore, ~MsgFIFODistributor";
		}
	} while (value != 1);

//Próba destrukcji semafora
	//TODO
	/*if (sem_destroy(&semaphore) < 0) {
		BOOST_LOG_TRIVIAL(warning) << "sem_destroy() failed, ~MsgFIFODistributor";
	}*/
	//TODO zniszczyc wszystkie semafory we wszystkich kolejkach
}

}

