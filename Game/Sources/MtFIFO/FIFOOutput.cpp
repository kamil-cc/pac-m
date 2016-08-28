/**
 * FIFOOutput.cpp
 *
 *  Created on: 28 sie 2016
 *      Author: Kamil Burzyñski
 */

#include <MtFIFO/FIFOOutput.hpp>

#include <boost/none.hpp>

mtfifo::FIFOOutput::FIFOOutput(const std::string& name){

}

mtfifo::FIFOOutput::~FIFOOutput(){

}

boost::any mtfifo::FIFOOutput::get(){
	return boost::none;
}

void mtfifo::FIFOOutput::put(boost::any elem){
	;
}

size_t mtfifo::FIFOOutput::size(){
	return 0;
}
