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
#include <typeinfo>
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
	boost::any a = input.get();
	boost::any any = 12;
	output.put(any);
	std::cout<< output.size() << std::endl;
	std::cout<< boost::any_cast<int>(input.get()) << std::endl;
	std::cout<< output.size() << std::endl;

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

	boost::any t = input.get();

	try{
		if(!t.empty()){
			const std::type_info &ti = t.type();
	    	std::cout << ti.name() << '\n';
			Ex2 ee = boost::any_cast<Ex2>(t);
			std::cout <<  ee.n << std::endl;
		}
	}catch (boost::bad_any_cast &e){
		std::cout << "Bad casting " << std::endl;
	}


	//Ex2 io = boost::any_cast<Ex2>(t);

	//std::cout<< io.n << std::endl;

	return 0;
}
