/**
 * main.cpp
 *
 *  Created on: 28 sie 2016
 *      Author: Kamil Burzy�ski
 */

//Std
#include <string>

//Boost
#include <boost/thread.hpp>

//App
//#include <MtFIFO/FIFO.hpp>
//#include <MtFIFO/FIFODistributor.hpp>
//#include <MtFIFO/FIFOInput.hpp>
//#include <MtFIFO/FIFOOutput.hpp>
#include <MtFIFO/Names.hpp>
#include <Threads/Logging/LoggingThread.hpp>

//Tmp
#include <iostream>
#include <Threads/FakeSender/FakeSender.hpp>

//using namespace mtfifo;
using namespace thd;

//Start aplikacji
int main() {

	//FIFODistributor& fifoDistributor = FIFODistributor::getInstance();
	//FIFO<FIFOInput> input = fifoDistributor.getFIFO<FIFOInput>(FIFO_LOG);
	//FIFO<FIFOOutput> output = fifoDistributor.getFIFO<FIFOOutput>(FIFO_LOG);
	//FIFO<FIFOInput> input2 = fifoDistributor.getFIFO<FIFOInput>("001");

/*#include <boost/any.hpp>
#include <boost/none.hpp>
	//POD - plain old data
	struct Ex2 {
	    int n;
	    Ex2(){}
	    Ex2(const Ex2& m){n = m.n;} // trivial and non-throwing
	};

	Ex2 ex2;
	ex2.n = 13;

	boost::any aa = ex2;*/



/*	output.put(aa);

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
*/
	LoggingThread log;
	boost::thread loggingThread(log);

	FakeSender send;
	boost::thread fakeSender(send);

	loggingThread.join();
	fakeSender.join();

	//FakeSender send2;
	//boost::thread fakeSender2(send2);

	/*std::cout << "end" << std::endl;*/

	//loggingThread.interrupt(); WYSY�AC wiadomo�c typu Exit
	//fakeSender.interrupt();
	//fakeSender2.interrupt();
	//fakeSender2.join();
	return 0;
}
