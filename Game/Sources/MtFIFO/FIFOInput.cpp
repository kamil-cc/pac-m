/**
 * FIFOInput.cpp
 *
 *  Created on: 28 sie 2016
 *      Author: Kamil Burzy�ski
 */

#include <MtFIFO/FIFOInput.hpp>

mtfifo::FIFOInput::FIFOInput(const std::string& name){

}

mtfifo::FIFOInput::~FIFOInput(){

}

boost::any mtfifo::FIFOInput::get(){
	boost::any any;
	//TODO zaimplementowac
	return any;
}

void mtfifo::FIFOInput::put(boost::any elem){
	//Nie podejmujemy �adnej akcji w przypadku pisania do koleji wej�ciowej
}

size_t mtfifo::FIFOInput::size(){
	return 0;
}
