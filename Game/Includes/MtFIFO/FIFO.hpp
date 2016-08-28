/**
 * FIFO.hpp
 *
 *  Created on: 27 sie 2016
 *      Author: Kamil Burzyñski
 */

#ifndef GAME_INCLUDES_MTFIFO_FIFO_HPP_
#define GAME_INCLUDES_MTFIFO_FIFO_HPP_

using namespace std;

namespace mtfifo{

template <class IO> class FIFO {
public:
	FIFO(){};
	virtual ~FIFO(){};
	inline void put() {io_fifo_.put();}; //Wrappery typu inline
	inline void get() {io_fifo_.get();};
private:
	IO io_fifo_;
};

}


#endif /* GAME_INCLUDES_MTFIFO_FIFO_HPP_ */
