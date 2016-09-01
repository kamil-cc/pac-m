/**
 * Types.hpp
 *
 *  Created on: 28 sie 2016
 *      Author: Kamil Burzyński
 *
 *  Plik zawiera typy wiadomości przesyłanych przez kolejki fifo.
 */

#ifndef GAME_INCLUDES_MTFIFO_TYPES_HPP_
#define GAME_INCLUDES_MTFIFO_TYPES_HPP_

//Std
#include <string>

//Boost
#include <boost/thread.hpp>

/**
 * Poziom logowania. Globalnie dostępny enum.
 */
typedef enum severity_log_level_{
	normal,
	notification,
	warning,
	error,
	critical
}severity_log_level;

namespace mtfifo{
	/**
	 * Prosty std::string
	 */
	struct StringElement{
		StringElement(const std::string& elem){
			value = elem;
		}
		StringElement(const StringElement& copy){
			value = copy.value;
		}
		std::string value;
	};

	/**
	 * Wiadomośc w logu
	 */
	struct LogElement{
		LogElement(const std::string& elem, const severity_log_level& lvl,
				const boost::thread::id& tid){
			value = elem;
			level = lvl;
			id = tid;
		}
		LogElement(const LogElement& copy){
			value = copy.value;
			level = copy.level;
			id = copy.id;
		}
		std::string value;
		severity_log_level level;
		boost::thread::id id;
	};

	/**
	 * Pusta struktura sygnalizująca koniecznośc kończenia wątku
	 */
	struct ExitThread{
	};
}

#endif /* GAME_INCLUDES_MTFIFO_TYPES_HPP_ */
