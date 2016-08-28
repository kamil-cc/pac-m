/**
 * FIFODistributor.hpp
 *
 *  Created on: 21 sie 2016
 *      Author: Kamil Burzyñski
 */

#ifndef GAME_INCLUDES_MTFIFO_FIFODISTRIBUTOR_HPP_
#define GAME_INCLUDES_MTFIFO_FIFODISTRIBUTOR_HPP_

//Standard
#include <string>

//Boost
//...

//App
#include <MtFIFO/FIFO.hpp>
#include <MtFIFO/FIFOInput.hpp>
#include <MtFIFO/FIFOOutput.hpp>

using namespace std;

namespace mtfifo{

/**
 * Dystrybutor kolejek fifo, singleton
 */
class FIFODistributor{
private:
	FIFODistributor();
	FIFODistributor(FIFODistributor const&); //Pusty
    void operator=(FIFODistributor const&); //Pusty

public:
	virtual ~FIFODistributor();
	static FIFODistributor& getInstance();
	/**
	 * Parametr szablonu mówi o tym czy kolejka jest "wejœciowa" (input),
	 * czy "wyjœciowa" (output).
	 * Argumentem funkcji jest nazwa kolejki.
	 */
	template<class IO> static FIFO<IO> getFIFO(const string& name){
		FIFO<IO> singleInstance_;
		return singleInstance_;
	}
};

}

#endif /* GAME_INCLUDES_MTFIFO_FIFODISTRIBUTOR_HPP_ */
