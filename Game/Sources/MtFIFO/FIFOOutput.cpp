/**
 * FIFOOutput.cpp
 *
 *  Created on: 28 sie 2016
 *      Author: Kamil Burzyñski
 */

#include <MtFIFO/FIFOOutput.hpp>

mtfifo::FIFOOutput::FIFOOutput(const std::string& name){
	FIFODistributor& fifoDistributor = FIFODistributor::getInstance();
	impl_ = const_cast<FIFOImplementation*>(fifoDistributor.getImplementation(name));
}

mtfifo::FIFOOutput::~FIFOOutput(){
}

fifo_t mtfifo::FIFOOutput::get(){
	fifo_t any;
	return any; //Zwracamy pusty element, nie mo¿na pobrac elementu z kolejki wyjœciowej
}

void mtfifo::FIFOOutput::put(fifo_t elem){
	impl_->put(elem);
}

size_t mtfifo::FIFOOutput::size(){
	return impl_->size();
}
