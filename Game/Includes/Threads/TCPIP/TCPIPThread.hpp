/*
 * TCPIPThread.hpp
 *
 *  Created on: 1 wrz 2016
 *      Author: Kamil Burzyñski
 */

#ifndef GAME_INCLUDES_THREADS_TCPIP_TCPIPTHREAD_HPP_
#define GAME_INCLUDES_THREADS_TCPIP_TCPIPTHREAD_HPP_

namespace thd{
	class TCPIPThread{
	public:
		void operator()(){ //TODO Wyrzucic treœc do pliku .cpp
			mtfifo::FIFODistributor& fifoDistributor = mtfifo::FIFODistributor::getInstance();
			//mtfifo::FIFO<mtfifo::FIFOInput> input
			//	= fifoDistributor.getFIFO<mtfifo::FIFOInput>(mtfifo::FIFO_TCP);
			mtfifo::FIFO<mtfifo::FIFOOutput> output
						= fifoDistributor.getFIFO<mtfifo::FIFOOutput>(mtfifo::FIFO_TCPIP);
			mtfifo::FIFO<mtfifo::FIFOOutput> log
									= fifoDistributor.getFIFO<mtfifo::FIFOOutput>(mtfifo::FIFO_LOG);

			boost::thread::id id = boost::this_thread::get_id();
			thd::ThreadRegistration& threadRegistration = thd::ThreadRegistration::getInstance();
			threadRegistration.registerThread(id, thd::TCPIP);

			ThreadTime& threadTime = ThreadTime::getInstance();
			timeMaster = threadTime.factory();

			boost::random::mt19937 rng;
			boost::random::uniform_int_distribution<> ten(1,10);

			while(1){
				/*wait = timeMaster->calculateTime(LOG_TIME, input.size());
				if(ten(rng) == 1){ //Wiadomoœc debugowa. Œrednio co dziesi¹ta
					elem = mtfifo::LogElement(
							std::string("Czekam: ") + boost::lexical_cast<std::string>(wait)
							+ " Rozmiar: " + boost::lexical_cast<std::string>(input.size()),
							normal,
							boost::this_thread::get_id());
					output.put(elem);
				}*/
				boost::this_thread::sleep_for(wait);
			}
		}

		virtual ~TCPIPThread(){ //TODO wstawic do pliku cpp
			delete timeMaster; //Mo¿e byc niebezpieczne, jeœli timeMaster nie bêdzie zainicjalizowany
		}

		private:
			TimeMaster *timeMaster;
			boost::chrono::milliseconds wait;
	};
}

#endif /* GAME_INCLUDES_THREADS_TCPIP_TCPIPTHREAD_HPP_ */
