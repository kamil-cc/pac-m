/**
 * FIFOInput.hpp
 *
 *  Created on: 27 sie 2016
 *      Author: Kamil Burzyñski
 */

#ifndef GAME_INCLUDES_MTFIFO_FIFOINPUT_HPP_
#define GAME_INCLUDES_MTFIFO_FIFOINPUT_HPP_

#include <string>
#include <boost/any.hpp>

namespace mtfifo{

class FIFOInput{
public:
	FIFOInput(const std::string& name);
	virtual ~FIFOInput();
	boost::any get();
	void put(boost::any elem);
	size_t size();
};

}

#endif /* GAME_INCLUDES_MTFIFO_FIFOINPUT_HPP_ */
