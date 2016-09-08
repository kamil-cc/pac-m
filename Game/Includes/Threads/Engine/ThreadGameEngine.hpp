/*
 * ThreadGameEngine.hpp
 *
 *  Created on: 5 wrz 2016
 *      Author: Kamil
 */

#ifndef GAME_INCLUDES_THREADS_ENGINE_THREADGAMEENGINE_HPP_
#define GAME_INCLUDES_THREADS_ENGINE_THREADGAMEENGINE_HPP_

#undef KEY_EVENT

//Boost
#include <boost/any.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/bind.hpp>
#include <boost/random.hpp>
#include <boost/thread.hpp>
#include <boost/variant.hpp>

//ncurses Windows
// http://invisible-island.net/ncurses/
extern "C"{
	#include <ncursesw/curses.h>
}

//App
#include <MtFIFO/FIFO.hpp>
#include <MtFIFO/FIFODistributor.hpp>
#include <MtFIFO/FIFOInput.hpp>
#include <MtFIFO/Names.hpp>
#include <MtFIFO/Types.hpp>
#include <Threads/ThreadNames.hpp>
#include <Threads/ThreadRegistration.hpp>
#include <Threads/ThreadTime.hpp>

namespace thd{



static const int GAME_ROWS = 25;
static const int GAME_COLS = 50;
static const int INFO_COLS = 30;
static const int TOTAL_COLS = GAME_COLS + INFO_COLS;

typedef enum move_e{UP, DOWN, LEFT, RIGHT, NONE} move_t;
typedef enum creature_e{PACMAN, GHOST1, GHOST2, GHOST4, LAST} creature_t;
typedef boost::variant<creature_t> creatureVariant_t;

//Arena sk³ada siê z dwóch typów
typedef boost::variant<char, chtype> arenaVariant_t;
static const arenaVariant_t d = 'd';
static const arenaVariant_t n = ' ';
static const arenaVariant_t M = 'M';
static const arenaVariant_t P = 'P';
static const arenaVariant_t S = 'S';
static const arenaVariant_t T = 'T';
static const arenaVariant_t O = 'O';

//Na wzór boost::array
typedef struct arena_s{std::vector<std::vector<arenaVariant_t> > value;} arena_t;

class GameEngine : public boost::static_visitor<>{ // : boost::static_visitor<void>
public:
	GameEngine();

	void printHelloInfo();
	void printPoints();
	void printArena(const arena_t& arena);
	void initEngine();
	void swapColors(int from, int to);
	void operator()(char c, int row, int col);
	void operator()(chtype c, int row, int col);
	void printPressed(int key);
	void callQuit();
	void printGoodbyeScreen(bool win);
	void moveGhost(int& row, int& col, bool& diamond, move_t& move);

	void movePacMan();
	void movePacMan(int row, int col);
	void moveGhost(int row, int col);

	void moveGhost(int up, int left, bool& diamond);
	void removeDiamond(int row, int col);
	void operator()();
private:
	//Czy terminal obs³uguje kolory
	bool hasColorsFlag_;
	//Sta³e na podstawie curses.h
	const chtype ULC = 4194412;
	const chtype LLC = 4194413;
	const chtype URC = 4194411;
	const chtype LRC = 4194410;
	const chtype HLI = 4194417;
	const chtype VLI = 4194424;
	const chtype BTE = 4194422;
	const chtype TTE = 4194423;
	const chtype LTE = 4194420;
	const chtype RTE = 4194421;
	const arena_t startingArena_ = { //struct //Arena od której zaczyna siê gra
		{ //vector
			{ULC, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, URC, ' ', ' ', ULC, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, URC},
			{VLI, 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', VLI, ' ', ' ', VLI, 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', VLI},
			{VLI, 'd', ULC, HLI, URC, 'd', ULC, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, URC, 'd', VLI, ' ', ' ', VLI, 'd', ULC, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, URC, 'd', ULC, HLI, URC, 'd', VLI},
			{VLI, 'd', LLC, HLI, LRC, 'd', LLC, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, LRC, 'd', LLC, HLI, HLI, LRC, 'd', LLC, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, LRC, 'd', LLC, HLI, LRC, 'd', VLI},
			{VLI, 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', VLI},
			{VLI, 'd', ULC, HLI, HLI, HLI, HLI, HLI, HLI, HLI, URC, 'd', ULC, HLI, URC, 'd', ULC, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, URC, 'd', ULC, HLI, URC, 'd', ULC, HLI, HLI, HLI, HLI, HLI, HLI, HLI, URC, 'd', VLI},
			{VLI, 'd', VLI, ' ', ' ', ' ', ' ', ' ', ' ', ' ', VLI, 'd', VLI, ' ', VLI, 'd', LLC, HLI, HLI, HLI, HLI, HLI, HLI, HLI, URC, ULC, HLI, HLI, HLI, HLI, HLI, HLI, HLI, LRC, 'd', VLI ,' ', VLI, 'd', VLI, ' ', ' ', ' ', ' ', ' ', ' ', ' ', VLI, 'd', VLI},
			{VLI, 'd', VLI, ' ', ' ', ' ', ' ', ' ', ' ', ' ', VLI, 'd', VLI, ' ', VLI, 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', VLI, VLI, 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', VLI, ' ', VLI, 'd', VLI, ' ', ' ', ' ', ' ', ' ', ' ', ' ', VLI, 'd', VLI},
			{VLI, 'd', VLI, ' ', ' ', ' ', ' ', ' ', ' ', ' ', VLI, 'd', VLI, ' ', VLI, 'd', HLI, HLI, HLI, HLI, HLI, HLI, HLI, 'd', LLC, LRC, 'd', HLI, HLI, HLI, HLI, HLI, HLI, HLI, 'd', VLI, ' ', VLI, 'd', VLI, ' ', ' ', ' ', ' ', ' ', ' ', ' ', VLI, 'd', VLI},
			{VLI, 'd', LLC, HLI, HLI, HLI, HLI, HLI, HLI, HLI, LRC, 'd', VLI, ' ', VLI, 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', VLI, ' ', VLI, 'd', LLC, HLI, HLI, HLI, HLI, HLI, HLI, HLI, LRC, 'd', VLI},
			{VLI, 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', VLI, ' ', VLI, 'd', ULC, HLI, HLI, HLI, HLI, '=', '=', '=', '=', '=', '=', '=', '=', HLI, HLI, HLI, HLI, URC, 'd', VLI, ' ', VLI, 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', VLI},
			{LLC, HLI, HLI, HLI, HLI, 'd', HLI, HLI, HLI, HLI, HLI, 'd', LLC, HLI, LRC, 'd', VLI, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', VLI, 'd', LLC, HLI, LRC, 'd', HLI, HLI, HLI, HLI, HLI, 'd', HLI, HLI, HLI, HLI, LRC},
			{' ', ' ', ' ', ' ', ' ', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', VLI, ' ', ' ', ' ', ' ', 'M', ' ', 'P', ' ', ' ', 'S', ' ', 'T', ' ', ' ', ' ', ' ', VLI, 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', ' ', ' ', ' ', ' ', ' '},
			{ULC, HLI, HLI, HLI, HLI, 'd', HLI, HLI, HLI, HLI, HLI, 'd', ULC, HLI, URC, 'd', VLI, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', VLI, 'd', ULC, HLI, URC, 'd', HLI, HLI, HLI, HLI, HLI, 'd', HLI, HLI, HLI, HLI, URC},
			{VLI, 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', VLI, ' ', VLI, 'd', LLC, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, LRC, 'd', VLI, ' ', VLI, 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', VLI},
			{VLI, 'd', ULC, HLI, HLI, HLI, HLI, HLI, HLI, HLI, URC, 'd', VLI, ' ', VLI, 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'O', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', VLI, ' ', VLI, 'd', ULC, HLI, HLI, HLI, HLI, HLI, HLI, HLI, URC, 'd', VLI},
			{VLI, 'd', VLI, ' ', ' ', ' ', ' ', ' ', ' ', ' ', VLI, 'd', VLI, ' ', VLI, 'd', HLI, HLI, HLI, HLI, HLI, HLI, HLI, 'd', ULC, URC, 'd', HLI, HLI, HLI, HLI, HLI, HLI, HLI, 'd', VLI, ' ', VLI, 'd', VLI, ' ', ' ', ' ', ' ', ' ', ' ', ' ', VLI, 'd', VLI},
			{VLI, 'd', VLI, ' ', ' ', ' ', ' ', ' ', ' ', ' ', VLI, 'd', VLI, ' ', VLI, 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', VLI, VLI, 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', VLI, ' ', VLI, 'd', VLI, ' ', ' ', ' ', ' ', ' ', ' ', ' ', VLI, 'd', VLI},
			{VLI, 'd', VLI, ' ', ' ', ' ', ' ', ' ', ' ', ' ', VLI, 'd', VLI, ' ', VLI, 'd', ULC, HLI, HLI, HLI, HLI, HLI, HLI, HLI, LRC, LLC, HLI, HLI, HLI, HLI, HLI, HLI, HLI, URC, 'd', VLI, ' ', VLI, 'd', VLI, ' ', ' ', ' ', ' ', ' ', ' ', ' ', VLI, 'd', VLI},
			{VLI, 'd', LLC, HLI, HLI, HLI, HLI, HLI, HLI, HLI, LRC, 'd', LLC, HLI, LRC, 'd', LLC, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, LRC, 'd', LLC, HLI, LRC, 'd', LLC, HLI, HLI, HLI, HLI, HLI, HLI, HLI, LRC, 'd', VLI},
			{VLI, 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', VLI},
			{VLI, 'd', ULC, HLI, URC, 'd', ULC, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, URC, 'd', ULC, HLI, HLI, URC, 'd', ULC, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, URC, 'd', ULC, HLI, URC, 'd', VLI},
			{VLI, 'd', LLC, HLI, LRC, 'd', LLC, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, LRC, 'd', VLI, ' ', ' ', VLI, 'd', LLC, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, LRC, 'd', LLC, HLI, LRC, 'd', VLI},
			{VLI, 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', VLI, ' ', ' ', VLI, 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', VLI},
			{LLC, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, LRC, ' ', ' ', LLC, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, HLI, LRC}
		} //vector
	}; //struct
	arena_t arena_; //Arena na której toczy siê gra
	int diamondsLeft_; //Licznik punktów
	int readedChar_; //Klawisz wciœniêty przez u¿ytkownika
	bool gameStarted_; //Czy gra altywna
	bool singlePlayer_; //Czy w trbie jednego gracza
	bool slave_; //Czy poruszamy siê duszkiem
	bool win_; //Flagi wygrania/przegrania gry
	bool lose_;
	bool sendRequest_; //Flaga ¿¹dania po³¹czenia do servera
	int pacManRow_; //Po³o¿enie PacMana
	int pacManCol_;
	int ghost1Row_;//Po³o¿enie Duchów
	int ghost1Col_;
	int ghost2Row_;
	int ghost2Col_;
	int ghost3Row_;
	int ghost3Col_;
	int ghost4Row_;
	int ghost4Col_;
	//
	bool ghost1Diamond_;
	bool ghost2Diamond_;
	bool ghost3Diamond_;
	bool ghost4Diamond_;
	//
	move_t ghost1Direction_;
	move_t ghost2Direction_;
	move_t ghost3Direction_;
	move_t ghost4Direction_;
	//
	bool gate_;
};

}

#endif /* GAME_INCLUDES_THREADS_ENGINE_THREADGAMEENGINE_HPP_ */
