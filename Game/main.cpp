/**
 * main.cpp
 *
 *  Created on: 28 sie 2016
 *      Author: Kamil Burzyñski
 */

//Std
#include <string>

//App
#include <MtFIFO/FIFO.hpp>
#include <MtFIFO/FIFODistributor.hpp>
#include <MtFIFO/FIFOInput.hpp>
#include <MtFIFO/FIFOOutput.hpp>

using namespace mtfifo;

//Start aplikacji
int main() {

	FIFODistributor& fifoDistributor = FIFODistributor::getInstance();
	FIFO<FIFOInput> input = fifoDistributor.getFIFO<FIFOInput>("000");
	FIFO<FIFOOutput> output = fifoDistributor.getFIFO<FIFOOutput>("000");
	FIFO<FIFOInput> input2 = fifoDistributor.getFIFO<FIFOInput>("001");

#include <boost/any.hpp>
	boost::any a = input.get();
	boost::any any = 12;
	output.put(any);
	std::cout<< output.size() << std::endl;
	std::cout<< boost::any_cast<int>(input.get()) << std::endl;
	std::cout<< output.size() << std::endl;

	return 0;
}
