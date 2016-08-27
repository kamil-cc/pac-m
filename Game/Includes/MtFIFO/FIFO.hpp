/**
 * FIFO.hpp
 *
 *  Created on: 27 sie 2016
 *      Author: Kamil Burzyñski
 */

#ifndef GAME_INCLUDES_MTFIFO_FIFO_HPP_
#define GAME_INCLUDES_MTFIFO_FIFO_HPP_

#include <boost/any.hpp>


using namespace std;
using namespace boost;

namespace mtfifo{

template <class IO> class FIFO {
public:
	FIFO(){};
	virtual ~FIFO(){};
	void put() {io_fifo_.get();};
	void get() {io_fifo_.put();};
private:
	IO io_fifo_;
};

}


#endif /* GAME_INCLUDES_MTFIFO_FIFO_HPP_ */
