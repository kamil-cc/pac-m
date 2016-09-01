/**
 * FIFOInput.cpp
 *
 *  Created on: 28 sie 2016
 *      Author: Kamil Burzyñski
 */

#include <MtFIFO/FIFOInput.hpp>

mtfifo::FIFOInput::FIFOInput(const std::string& name){
	FIFODistributor& fifoDistributor = FIFODistributor::getInstance();
	impl_ = const_cast<FIFOImplementation*>(fifoDistributor.getImplementation(name));
	if(!impl_->isInputAvailable())
		assert(!"More than one input queue with the same name");
	impl_->setUnavailable();
}

mtfifo::FIFOInput::~FIFOInput(){
}

boost::any mtfifo::FIFOInput::get(){
	return impl_->get();
}

void mtfifo::FIFOInput::put(boost::any elem){
	//Nie podejmujemy ¿adnej akcji w przypadku pisania do koleji wejœciowej
}

std::size_t mtfifo::FIFOInput::size(){
	return impl_->size();
}
