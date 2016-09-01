/**
 * FIFOInput.hpp
 *
 *  Created on: 27 sie 2016
 *      Author: Kamil Burzy�ski
 */

#ifndef GAME_INCLUDES_MTFIFO_FIFOINPUT_HPP_
#define GAME_INCLUDES_MTFIFO_FIFOINPUT_HPP_

//Std
#include <string>

//Boost
#include <boost/any.hpp>

//App
#include <MtFIFO/FIFODistributor.hpp>
#include <MtFIFO/FIFOImplementation.hpp>

namespace mtfifo{

class FIFOInput{
public:
	FIFOInput(const std::string& name);
	virtual ~FIFOInput();
	boost::any get();
	void put(boost::any elem);
	std::size_t size();
private:
	FIFOImplementation* impl_;
};

}

#endif /* GAME_INCLUDES_MTFIFO_FIFOINPUT_HPP_ */
