/*
 * Types.hpp
 *
 *  Created on: 28 sie 2016
 *      Author: Kamil
 */

#ifndef GAME_INCLUDES_MTFIFO_TYPES_HPP_
#define GAME_INCLUDES_MTFIFO_TYPES_HPP_

#include <string>

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
}

#endif /* GAME_INCLUDES_MTFIFO_TYPES_HPP_ */
