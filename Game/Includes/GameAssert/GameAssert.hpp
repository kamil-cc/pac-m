/*
 * GameAssert.hpp
 *
 *  Created on: 11 wrz 2016
 *      Author: Kamil
 */

#ifndef GAME_INCLUDES_GAMEASSERT_GAMEASSERT_HPP_
#define GAME_INCLUDES_GAMEASSERT_GAMEASSERT_HPP_

#include <cassert>
#include <ncursesw/ncurses.h>


#define gameAssert(expr) \
	do{ \
		clear(); \
		assert(expr); \
	}while(0) \


#endif /* GAME_INCLUDES_GAMEASSERT_GAMEASSERT_HPP_ */
