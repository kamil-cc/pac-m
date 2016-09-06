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
#include <boost/bind.hpp>
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

namespace thd{

const int GAME_ROWS = 25; //TODO do sk³adowych statycznych klasy GameEngine
const int GAME_COLS = 50;
const int INFO_COLS = 30;
const int TOTAL_COLS = GAME_COLS + INFO_COLS;
typedef boost::variant<char, chtype> arenaVariant_t;

//Straszne haki, takie jak boost::array
typedef struct arena_s{std::vector<std::vector<arenaVariant_t> > value;} arena_t;

class GameEngine : public boost::static_visitor<>{ // : boost::static_visitor<void>
public:
	void printHelloInfo(){
		const int ANCHOR = 12;
		int row = 0;
		mvprintw(++row, GAME_COLS + 1, "         Hello!");
		mvprintw(++row, GAME_COLS + 1, "For Single Player press ' '");
		swapColors(1, 2);
		mvprintw(row, GAME_COLS + 1 + 25, "S");
		swapColors(2, 1);
		mvprintw(++row, GAME_COLS + 1, "For Multi Player press ' '");
		swapColors(1, 2);
		mvprintw(row, GAME_COLS + 1 + 24, "M");
		swapColors(2, 1);
		mvprintw(++row, GAME_COLS + 1, "");
		mvprintw(++row, GAME_COLS + 1, "        Controls:");
		mvaddch(++row, GAME_COLS + 1 + ANCHOR, ACS_UARROW);
		mvaddch(++row, GAME_COLS + 1 + ANCHOR, ACS_DARROW);
		mvaddch(row, GAME_COLS + 1 + ANCHOR - 1, ACS_LARROW);
		mvaddch(row, GAME_COLS + 1 + ANCHOR + 1, ACS_RARROW);
		mvprintw(++row, GAME_COLS + 1, "");
		mvprintw(++row, GAME_COLS + 1, "        Points:");
		swapColors(1, 3);
		mvprintw(row, GAME_COLS + 1 + 16, "0");
		swapColors(3, 1);
		refresh();
	}

	void printArena(){
		for(int row = 0; row < static_cast<int>(startingArena_.value.size()); ++row){
			for(int col = 0; col < static_cast<int>(startingArena_.value[0].size()); ++col){
				auto bindedVisitor = boost::bind(*this, _1, row, col);
				boost::apply_visitor(bindedVisitor, startingArena_.value[row][col]);
			}
			refresh();
		}
		refresh();
	}

	void initEngine(){
		initscr();
		cbreak();
		int maxRows = 0;
		int maxCols = 0;
		if(has_colors() == FALSE){
			hasColorsFlag_ = false;
		}else{
			hasColorsFlag_ = true;
			start_color();
			init_pair(1, COLOR_WHITE, COLOR_BLACK);
			init_pair(2, COLOR_GREEN, COLOR_BLACK);
			init_pair(3, COLOR_BLUE, COLOR_BLACK);
			init_pair(4, COLOR_YELLOW, COLOR_BLACK);
			init_pair(5, COLOR_CYAN, COLOR_BLACK);
			init_pair(6, COLOR_MAGENTA, COLOR_BLACK);
			init_pair(7, COLOR_RED, COLOR_BLACK);
			attron(COLOR_PAIR(1));
		}
		curs_set(0);
		getmaxyx(stdscr, maxRows, maxCols);
		if((maxRows == 0) || (maxRows < GAME_ROWS)){
			endwin();
			assert(!"Wrong rows num");
		}
		if((maxCols == 0) || (maxCols < TOTAL_COLS)){
			endwin();
			assert(!"Wrong cols num");
		}
		if(resize_term(GAME_ROWS, TOTAL_COLS) == ERR){
			endwin();
			assert(!"resize_term failed");
		}
		printHelloInfo();
		printArena();
		getch();
		endwin();
	}

	void swapColors(int from, int to){
		if(hasColorsFlag_){
			attroff(COLOR_PAIR(from));
			attron(COLOR_PAIR(to));
		}
	}

	void operator()(char c, int row, int col){
		char ch = std::tolower(c);
		switch(ch){
		case'd':
			mvaddch(row, col, ACS_BULLET);
			break;
		case'o':
			swapColors(1, 4);
			mvaddch(row, col, 'O');
			swapColors(4, 1);
			break;
		case '=':
			swapColors(1, 7);
			mvaddch(row, col, '=');
			swapColors(7, 1);
			break;
		case 'm':
			swapColors(1, 6);
			mvaddch(row, col, 'M');
			swapColors(6, 1);
			break;
		default:
			break;
		}
	}

	void operator()(chtype c, int row, int col){
		if(((((c == ULC || c == LLC)) || (c == URC || c == LRC))
			|| ((c == HLI || c == VLI) || (c == TTE || c == BTE)))
				|| (c == LTE || c == RTE)){
				swapColors(1, 3);
				mvaddch(row, col, c);
				swapColors(3, 1);
		}
	}

	//Punkt wejœcia w¹tku
	void operator()(){
		initEngine();
		/*mtfifo::FIFODistributor& fifoDistributor = mtfifo::FIFODistributor::getInstance();
		//mtfifo::FIFO<mtfifo::FIFOOutput> output
		//	= fifoDistributor.getFIFO<mtfifo::FIFOOutput>(mtfifo::FIFO_LOG);
		mtfifo::FIFO<mtfifo::FIFOOutput> output
					= fifoDistributor.getFIFO<mtfifo::FIFOOutput>(mtfifo::FIFO_SERIALIZED_OUTPUT);

		boost::thread::id id = boost::this_thread::get_id();
		thd::ThreadRegistration& threadRegistration = thd::ThreadRegistration::getInstance();
		threadRegistration.registerThread(id, thd::FAKE);*/

		while(1){
			/*std::string str = "Wiadomosc nr: " + boost::lexical_cast<std::string>(i);
			//boost::any elem = mtfifo::LogElement(str, normal, boost::this_thread::get_id());
			boost::any elem = mtfifo::TCPIPSerialized(str);
			output.put(elem);*/

			boost::this_thread::sleep_for(boost::chrono::milliseconds(50));
		}
	}
private:
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
	const arena_t startingArena_ = { //struct
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
			{' ', ' ', ' ', ' ', ' ', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', VLI, ' ', ' ', ' ', ' ', 'M', ' ', 'M', ' ', ' ', 'M', ' ', 'M', ' ', ' ', ' ', ' ', VLI, 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', ' ', ' ', ' ', ' ', ' '},
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
	arena_t arena_ = startingArena_;
};

}

#endif /* GAME_INCLUDES_THREADS_ENGINE_THREADGAMEENGINE_HPP_ */
