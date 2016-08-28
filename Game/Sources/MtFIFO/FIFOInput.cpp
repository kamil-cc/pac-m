/**
 * FIFOInput.cpp
 *
 *  Created on: 28 sie 2016
 *      Author: Kamil Burzy�ski
 */

#include <MtFIFO/FIFOInput.hpp>

mtfifo::FIFOInput::FIFOInput(const std::string& name){
	FIFODistributor& fifoDistributor = FIFODistributor::getInstance();
	impl_ = const_cast<FIFOImplementation*>(fifoDistributor.getImplementation(name));
}

mtfifo::FIFOInput::~FIFOInput(){
}

fifo_t mtfifo::FIFOInput::get(){
	return impl_->get();
}

void mtfifo::FIFOInput::put(fifo_t elem){
	//Nie podejmujemy �adnej akcji w przypadku pisania do koleji wej�ciowej
}

size_t mtfifo::FIFOInput::size(){
	return impl_->size();
}
