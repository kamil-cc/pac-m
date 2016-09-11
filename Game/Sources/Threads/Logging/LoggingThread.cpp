/**
 * LoggingThread.cpp
 *
 *  Created on: 11 wrz 2016
 *      Author: Kamil Burzyñski
 */

#include <Threads/Logging/LoggingThread.hpp>

//Operator wyœwietlaj¹cy poziom loga w zrozumia³ej dla cz³owieka postaci
std::ostream& operator<<(std::ostream& strm, severity_log_level level){
    static const char* strings[] = {
        "[NORMAL]",
        "[NOTIFICATION]",
        "[WARNING]",
        "[ERROR]",
        "[CRITICAL]"
    };

    if (static_cast<std::size_t>(level) < (sizeof(strings) / sizeof(*strings)))
        strm << strings[level];
    else
        strm << static_cast<int>(level);

    return strm;
}

void thd::LoggingThread::loggingInit() {
	boost::log::register_simple_formatter_factory<severity_log_level, char>(
			"Severity");
	//W tym miejscu mo¿na regulowac poziom intensywnoœci loga.
	boost::log::core::get()->set_filter(
			boost::log::expressions::attr<severity_log_level>("Severity")
					>= /*normal*/warning);
	//Log do pliku
	boost::log::add_file_log(boost::log::keywords::file_name = "game.log",
			boost::log::keywords::format = "%Severity% %Message%",
			boost::log::keywords::auto_flush = true);
	//Log na konsolê
	//boost::log::add_console_log(
	//		std::cout,
	//		boost::log::keywords::format = "%Severity% %Message%",
	//		boost::log::keywords::auto_flush = true);
}

void thd::LoggingThread::operator()() {
	loggingInit();
	boost::log::sources::severity_logger<severity_log_level> log;

	mtfifo::FIFODistributor& fifoDistributor =
			mtfifo::FIFODistributor::getInstance();
	mtfifo::FIFO<mtfifo::FIFOInput> input = fifoDistributor.getFIFO<
			mtfifo::FIFOInput>(mtfifo::FIFO_LOG);
	mtfifo::FIFO<mtfifo::FIFOOutput> output = fifoDistributor.getFIFO<
			mtfifo::FIFOOutput>(mtfifo::FIFO_LOG);

	boost::thread::id id = boost::this_thread::get_id();
	thd::ThreadRegistration& threadRegistration =
			thd::ThreadRegistration::getInstance();
	threadRegistration.registerThread(id, thd::LOGGER);

	ThreadTime& threadTime = ThreadTime::getInstance();
	timeMasterPtr_ = threadTime.factory();

	boost::random::mt19937 rng;
	boost::random::uniform_int_distribution<> ten(1, 10);

	while (1) {
		boost::any elem = input.get();
		try {
			mtfifo::LogElement logElement = boost::any_cast<mtfifo::LogElement>(
					elem);
			BOOST_LOG_SEV(log, logElement.level)<< "[" << threadRegistration.getName(logElement.id)
					<< "] " << logElement.value;
		} catch(boost::bad_any_cast &e) {
			try {
				boost::any_cast<boost::none_t>(elem);
			} catch(boost::bad_any_cast &e) {
				try {
					boost::any_cast<mtfifo::ExitThread>(elem);
					break;
				} catch(boost::bad_any_cast &e) {
					gameAssert(!"Unknown element type");
				}
			}
		}
		wait_ = timeMasterPtr_->calculateTime(LOG_TIME, input.size());
		if (ten(rng) == 1) { //Wiadomoœc debugowa. Œrednio co dziesi¹ta
			elem = mtfifo::LogElement(
					std::string("Czekam: ")
							+ boost::lexical_cast<std::string>(wait_)
							+ " Rozmiar: "
							+ boost::lexical_cast<std::string>(input.size()),
					normal, boost::this_thread::get_id());
			output.put(elem);
		}
		boost::this_thread::sleep_for(wait_);
	}
}

thd::LoggingThread::~LoggingThread() {
	delete timeMasterPtr_; //Mo¿e byc niebezpieczne, jeœli timeMaster nie bêdzie zainicjalizowany
}

