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
#include <MtFIFO/FIFODistributor.hpp>

//Start aplikacji
int main() {

	mtfifo::FIFODistributor& FIFODistributor = mtfifo::FIFODistributor::getInstance();
	FIFODistributor.getFIFO<mtfifo::FIFOInput>("000");
	FIFODistributor.getFIFO<mtfifo::FIFOOutput>("000");
	FIFODistributor.getFIFO<mtfifo::FIFOInput>("001");

	return 0;
}
