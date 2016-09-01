/**
 * FIFODistributor.hpp
 *
 *  Created on: 21 sie 2016
 *      Author: Kamil Burzyñski
 */

#ifndef GAME_INCLUDES_MTFIFO_FIFODISTRIBUTOR_HPP_
#define GAME_INCLUDES_MTFIFO_FIFODISTRIBUTOR_HPP_

//Std
#include <string>
#include <map>
#include <utility> //pair

//Boost
#include <boost/thread/lock_guard.hpp>
#include <boost/thread/lockable_adapter.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/noncopyable.hpp>

//App
#include <MtFIFO/FIFO.hpp>
#include <MtFIFO/FIFOImplementation.hpp>
#include <MtFIFO/FIFOInput.hpp>
#include <MtFIFO/FIFOOutput.hpp>

namespace mtfifo{

/**
 * Dystrybutor kolejek fifo, singleton
 */
class FIFODistributor : private boost::noncopyable,
	public boost::basic_lockable_adapter<boost::recursive_mutex>{
	friend class FIFOInput;
	friend class FIFOOutput;
private:
	std::map<std::string, const FIFOImplementation*> fifoMap_;
	FIFODistributor(); //Pusty
	FIFODistributor(FIFODistributor const&); //Pusty
	const FIFOImplementation* getImplementation(const std::string& name); //zabezpieczone guardem jak poni¿sza funkcja

public:
	virtual ~FIFODistributor();
	static FIFODistributor& getInstance();
	/**
	 * Parametr szablonu mówi o tym czy kolejka jest "wejœciowa" (input),
	 * czy "wyjœciowa" (output).
	 * Argumentem funkcji jest nazwa kolejki.
	 */
	template<class IO> FIFO<IO> getFIFO(const std::string& name){
		boost::lock_guard<FIFODistributor> guard(*this);
		FIFO<IO> fifo_(name);
		return fifo_;
	}
};

}

#endif /* GAME_INCLUDES_MTFIFO_FIFODISTRIBUTOR_HPP_ */
