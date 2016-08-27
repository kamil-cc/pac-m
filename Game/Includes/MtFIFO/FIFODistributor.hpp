/**
 * FIFODistributor.hpp
 *
 *  Created on: 21 sie 2016
 *      Author: Kamil Burzyñski
 */

#ifndef GAME_INCLUDES_MTFIFO_FIFODISTRIBUTOR_HPP_
#define GAME_INCLUDES_MTFIFO_FIFODISTRIBUTOR_HPP_

//Standard
#include <iostream>
#include <cstring>
#include <map>
#include <typeinfo>
#include <sys/types.h> //Wymagany przez semaphore
//#include <semaphore.h>

//Boost
//#include <boost/thread/mutex.hpp>
#include <boost/log/trivial.hpp> //TODO Mo¿e nie jest potrzebny?
#include <MtFIFO/FIFO.hpp>
#include <MtFIFO/FIFOInput.hpp>
#include <MtFIFO/FIFOOutput.hpp>

//App

using namespace std;
using namespace boost::log::trivial;

namespace mtfifo{

/**
 * Dystrybutor kolejek fifo, singleton
 */
class FIFODistributor{
private:
	FIFODistributor();
	FIFODistributor(FIFODistributor const&); //Pusty
    void operator=(FIFODistributor const&); //Pusty //TODO zainicjalizowac semafor??? Mo¿e prowadzic do b³êdów

public:
	virtual ~FIFODistributor();
	static FIFODistributor& getInstance();
	/**
	 * Parametr szablonu mówi o tym czy kolejka jest "wejœciowa" (input),
	 * czy "wyjœciowa" (output).
	 * Argumentem funkcji jest nazwa kolejki.
	 */
	template<class IO> static FIFO<IO> getFIFO(const string& name){
		FIFO<IO> fxb/*(name)*/; //New FIFO on stack
		return fxb;
	}
};

}

#endif /* GAME_INCLUDES_MTFIFO_FIFODISTRIBUTOR_HPP_ */
