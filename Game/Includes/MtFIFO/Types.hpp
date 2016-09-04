/**
 * Types.hpp
 *
 *  Created on: 28 sie 2016
 *      Author: Kamil Burzyñski
 *
 *  Plik zawiera typy wiadomoœci przesy³anych przez kolejki fifo.
 */

#ifndef GAME_INCLUDES_MTFIFO_TYPES_HPP_
#define GAME_INCLUDES_MTFIFO_TYPES_HPP_

//Std
#include <string>

//Boost
#include <boost/thread.hpp>

/**
 * Poziom logowania. Globalnie dostêpny enum.
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
	 * Wiadomoœc w logu
	 */
	struct LogElement{
		LogElement(){
			value.clear();
			level = normal;
		}
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
		LogElement operator<<(const boost::thread::id& setId){
			id = setId;
			value.clear();
			level = normal;
			return *this;
		}
		LogElement operator<<(const severity_log_level& setLevel){
			level = setLevel;
			return *this;
		}
		LogElement operator<<(const std::string& setValue){
			value = std::string(setValue);
			return *this;
		}
		std::string value;
		severity_log_level level;
		boost::thread::id id;
	};

	struct TCPIPSerialized{
		TCPIPSerialized(const std::string& serial){
			serialized = serial;
		}
		TCPIPSerialized(const TCPIPSerialized& copy){
			serialized = copy.serialized;
		}
		std::string serialized;
	};

	/**
	 * Pusta struktura sygnalizuj¹ca koniecznoœc koñczenia w¹tku
	 */
	struct ExitThread{
	};
}

#endif /* GAME_INCLUDES_MTFIFO_TYPES_HPP_ */
