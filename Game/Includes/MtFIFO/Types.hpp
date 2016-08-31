/*
 * Types.hpp
 *
 *  Created on: 28 sie 2016
 *      Author: Kamil
 */

#ifndef GAME_INCLUDES_MTFIFO_TYPES_HPP_
#define GAME_INCLUDES_MTFIFO_TYPES_HPP_

//Std
#include <string>

//Boost
#include <boost/log/trivial.hpp>

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
	struct StringElement{
		StringElement(const std::string& elem){
			value = elem;
		}
		StringElement(const StringElement& copy){
			value = copy.value;
		}
		std::string value;
	};

	struct LogElement{
		LogElement(const std::string& elem, boost::log::trivial::severity_level lvl){
			value = elem;
			level = lvl;
		}
		LogElement(const LogElement& copy){
			value = copy.value;
			level = copy.level;
		}
		std::string value;
		boost::log::trivial::severity_level level;
	};

	struct ExitThread{
		//Pusta struktura sygnalizuj¹ca koniecznoœc koñczenia w¹tku
	};
}

#endif /* GAME_INCLUDES_MTFIFO_TYPES_HPP_ */
