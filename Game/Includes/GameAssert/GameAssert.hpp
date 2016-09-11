/*
 * GameAssert.hpp
 *
 *  Created on: 11 wrz 2016
 *      Author: Kamil
 */

#ifndef GAME_INCLUDES_GAMEASSERT_GAMEASSERT_HPP_
#define GAME_INCLUDES_GAMEASSERT_GAMEASSERT_HPP_

#include <cassert>
#include <string>
#include <iostream>
#include <ncursesw/ncurses.h>

extern bool isNcursesMode;

// https://gcc.gnu.org/onlinedocs/cpp/Stringification.html
#define gameAssert(expr) \
	do{ \
		if(isNcursesMode){ \
			endwin(); \
			isNcursesMode = false; \
		} \
		std::string str(#expr); \
		std::cout << "Assertion Failed: " << str << std::endl; \
		exit(-1); \
	}while(0)


#endif /* GAME_INCLUDES_GAMEASSERT_GAMEASSERT_HPP_ */
