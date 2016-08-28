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

#include <CommonTypes.hpp>
#include <boost/any.hpp>
#include <boost/none.hpp>
	fifo_t a = input.get();
	fifo_t any = 12;
	output.put(any);
	std::cout<< output.size() << std::endl;
	std::cout<< boost::any_cast<int>(input.get()) << std::endl;
	std::cout<< output.size() << std::endl;

	//POD - plain old data
	class o{
	public:
		int b;
		o(){
			b = 13;
		}
		o(const o& a){
			b = a.b;
		}
	};

	o oo;

	//fifo_t aa = oo;

	//output.put(a);

	//o io = fifo_t_cast<o>(input.get());

	//std::cout<< io.b << std::endl;

	return 0;
}
