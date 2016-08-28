/*
 * FIFOUnderHood.hpp
 *
 *  Created on: 28 sie 2016
 *      Author: Kamil
 */

#ifndef GAME_INCLUDES_MTFIFO_FIFOIMPLEMENTATION_HPP_
#define GAME_INCLUDES_MTFIFO_FIFOIMPLEMENTATION_HPP_

//Std
#include <queue>

//Boost
#include <boost/any.hpp>
#include <boost/thread/lock_guard.hpp>
#include <boost/thread/lockable_adapter.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/none.hpp>

//App
#include <CommonTypes.hpp>

using namespace std;

namespace mtfifo{

class FIFOImplementation : public boost::basic_lockable_adapter<boost::mutex>{
public:
	FIFOImplementation();
	virtual ~FIFOImplementation();
	fifo_t get();
	void put(fifo_t elem);
	size_t size();
private:
	static size_t MAX_QUEUE_SIZE;
	queue<fifo_t> queue_;
};

}

#endif /* GAME_INCLUDES_MTFIFO_FIFOIMPLEMENTATION_HPP_ */
