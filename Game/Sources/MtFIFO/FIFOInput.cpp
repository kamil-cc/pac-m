/**
 * FIFOInput.cpp
 *
 *  Created on: 28 sie 2016
 *      Author: Kamil Burzyñski
 */

#include <MtFIFO/FIFOInput.hpp>

#include <boost/none.hpp>

mtfifo::FIFOInput::FIFOInput(const std::string& name){

}

mtfifo::FIFOInput::~FIFOInput(){

}

boost::any mtfifo::FIFOInput::get(){
	return boost::none;
}

void mtfifo::FIFOInput::put(boost::any elem){
	;
}

size_t mtfifo::FIFOInput::size(){
	return 0;
}
