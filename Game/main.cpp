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

//Del
#include <iostream>

using namespace mtfifo;

//Start aplikacji
int main() {

	FIFODistributor& fifoDistributor = FIFODistributor::getInstance();
	FIFO<FIFOInput> input = fifoDistributor.getFIFO<FIFOInput>("000");
	FIFO<FIFOOutput> output = fifoDistributor.getFIFO<FIFOOutput>("000");
	FIFO<FIFOInput> input2 = fifoDistributor.getFIFO<FIFOInput>("001");

#include <boost/any.hpp>
#include <boost/none.hpp>
	//POD - plain old data
	struct Ex2 {
	    int n;
	    Ex2(){}
	    Ex2(const Ex2& m){n = m.n;} // trivial and non-throwing
	};

	Ex2 ex2;
	ex2.n = 13;

	boost::any aa = ex2;

	output.put(aa);

	for(int i = 0; i < 2; ++i){
		boost::any t = input.get();
		try{
			Ex2 ee = boost::any_cast<Ex2>(t);
			std::cout <<  ee.n << std::endl;
		}catch (boost::bad_any_cast &e){
			std::cout << "Bad casting " << std::endl;
		}
		try{
			boost::any_cast<boost::none_t>(t);
			std::cout <<  "None" << std::endl;
		}catch (boost::bad_any_cast &e){
			std::cout << "Bad casting " << std::endl;
		}
	}

	return 0;
}
