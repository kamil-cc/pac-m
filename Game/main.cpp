/**
 * main.cpp
 *
 *  Created on: 28 sie 2016
 *      Author: Kamil Burzyñski
 */

//Standard
#include <string>

//Boost
//...

//App
#include <MtFIFO/FIFO.hpp>
#include <MtFIFO/FIFODistributor.hpp>
#include <MtFIFO/FIFOInput.hpp>
#include <MtFIFO/FIFOOutput.hpp>

using namespace mtfifo;

//Start aplikacji
int main() {

	FIFODistributor& FIFODistributor = FIFODistributor::getInstance();
	FIFO<FIFOInput> input = FIFODistributor.getFIFO<FIFOInput>("000");
	FIFO<FIFOOutput> output = FIFODistributor.getFIFO<FIFOOutput>("000");
	FIFO<FIFOInput> input2 = FIFODistributor.getFIFO<FIFOInput>("001");

	input.get();

	return 0;
}
