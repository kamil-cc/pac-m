/**
 * FIFOOutput.cpp
 *
 *  Created on: 28 sie 2016
 *      Author: Kamil Burzyñski
 */

#include <MtFIFO/FIFOOutput.hpp>

mtfifo::FIFOOutput::FIFOOutput(const std::string& name){
	FIFODistributor& fifoDistributor = FIFODistributor::getInstance();
	fifoDistributor.getImplementation(name);
}

mtfifo::FIFOOutput::~FIFOOutput(){

}

boost::any mtfifo::FIFOOutput::get(){
	boost::any any;
	return any; //Zwracamy pusty element
}

void mtfifo::FIFOOutput::put(boost::any elem){
	;//TODO zaimplementowac
}

size_t mtfifo::FIFOOutput::size(){
	return 0;
}
