/*
 * FIFOImplementation.cpp
 *
 *  Created on: 28 sie 2016
 *      Author: Kamil
 */

#include <MtFIFO/FIFOImplementation.hpp>

size_t mtfifo::FIFOImplementation::MAX_QUEUE_SIZE = 1024; //Arbitralnie dobrana wielko�c

mtfifo::FIFOImplementation::FIFOImplementation(){
}

mtfifo::FIFOImplementation::~FIFOImplementation(){

}

boost::any mtfifo::FIFOImplementation::get(){
	boost::lock_guard<FIFOImplementation> guard(*this);
	boost::any any = queue_.front();
	queue_.pop();
	return any;
}

void mtfifo::FIFOImplementation::put(boost::any elem){
	boost::lock_guard<FIFOImplementation> guard(*this);
	if(queue_.size() > MAX_QUEUE_SIZE)
		assert(!"Queue is too big"); //zabezpieczenie
	queue_.push(elem);
}

size_t mtfifo::FIFOImplementation::size(){
	boost::lock_guard<FIFOImplementation> guard(*this);
	return queue_.size();
}