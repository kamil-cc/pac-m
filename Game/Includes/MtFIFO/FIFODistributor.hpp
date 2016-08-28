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
#include <boost/thread/lock_guard.hpp>
#include <boost/thread/lockable_adapter.hpp>
#include <boost/thread/mutex.hpp>

//App
#include <MtFIFO/FIFO.hpp>
#include <MtFIFO/FIFOInput.hpp>
#include <MtFIFO/FIFOOutput.hpp>

using namespace std;

namespace mtfifo{

/**
 * Dystrybutor kolejek fifo, singleton
 */
class FIFODistributor : public boost::basic_lockable_adapter<boost::mutex>{
private:
	FIFODistributor();
	FIFODistributor(FIFODistributor const&); //Pusty

public:
	virtual ~FIFODistributor();
	static FIFODistributor& getInstance();
	/**
	 * Parametr szablonu mówi o tym czy kolejka jest "wejœciowa" (input),
	 * czy "wyjœciowa" (output).
	 * Argumentem funkcji jest nazwa kolejki.
	 */
	template<class IO> FIFO<IO> getFIFO(const string& name){
		boost::lock_guard<FIFODistributor> guard(*this);
		FIFO<IO> fifo_(name);
		return fifo_;
	}
};

}

#endif /* GAME_INCLUDES_MTFIFO_FIFODISTRIBUTOR_HPP_ */
