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
#include <boost/log/trivial.hpp>
#include <boost/thread.hpp>

//TODO usuñ
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>

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
	 * Poziom logowania
	 */
	typedef enum severity_log_level_{
	    normal,
	    notification,
	    warning,
	    error,
	    critical
	}severity_log_level;

	/**
	 * Wiadomoœc w logu
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
	 * Pusta struktura sygnalizuj¹ca koniecznoœc koñczenia w¹tku
	 */
	struct ExitThread{
	};
}

#endif /* GAME_INCLUDES_MTFIFO_TYPES_HPP_ */
