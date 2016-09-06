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

//Trzyliterowe aliasy dla przejrzystoœci macierzy na dole pliku
#define LLC ACS_LLCORNER
#define ULC ACS_ULCORNER
#define LRC ACS_LRCORNER
#define URC ACS_URCORNER
#define HLI ACS_HLINE
#define VLI ACS_VLINE

class GameEngine : public boost::static_visitor<>{ // : boost::static_visitor<void>
public:
	void printFrame(){
		for(int col = 1; col < GAME_COLS - 1; ++col){ //Linia górna
			mvaddch(0, col, ACS_HLINE);
		}
		for(int col = 1; col < GAME_COLS - 1; ++col){ //Linia dolna
			mvaddch(GAME_ROWS - 1, col, ACS_HLINE);
		}
		for(int row = 1; row < GAME_ROWS - 1; ++row){ //Linia lewa
			if(row == ((GAME_ROWS - 1) / 2))
				;
			else if(row == (((GAME_ROWS - 1) / 2) - 1))
				mvaddch(row, 0, ACS_LLCORNER);
			else if(row == (((GAME_ROWS - 1) / 2) + 1))
				mvaddch(row, 0, ACS_ULCORNER);
			else
				mvaddch(row, 0, ACS_VLINE);
		}
		for(int row = 1; row < GAME_ROWS - 1; ++row){ //Linia prawa
			if(row == ((GAME_ROWS - 1) / 2))
				;
			else if(row == (((GAME_ROWS - 1) / 2) - 1))
				mvaddch(row, GAME_COLS - 1, ACS_LRCORNER);
			else if(row == (((GAME_ROWS - 1) / 2) + 1))
				mvaddch(row, GAME_COLS - 1, ACS_URCORNER);
			else
				mvaddch(row, GAME_COLS - 1, ACS_VLINE);
		}
		mvaddch(0, 0, ACS_ULCORNER);
		mvaddch(GAME_ROWS - 1, 0, ACS_LLCORNER);
		mvaddch(0, GAME_COLS - 1, ACS_URCORNER);
		mvaddch(GAME_ROWS - 1, GAME_COLS - 1, ACS_LRCORNER);
		swapColors(3, 1);
		refresh();
	}

	void printHelloInfo(){
		const int ANCHOR = 12;
		int row = 0;
		mvprintw(++row, GAME_COLS + 1, "         Hello!");
		mvprintw(++row, GAME_COLS + 1, "For Single Player press ' '");
		swapColors(3, 2);
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
				auto bindedVisitor = boost::bind(GameEngine(), _1, row + 1, col + 1);
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
			attron(COLOR_PAIR(3));
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
		refresh();
		printFrame();
		printHelloInfo();
		printArena();
		refresh();
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
		switch(c){
		case'd':
			swapColors(1, 4);
			mvaddch(row, col, ACS_DIAMOND);
			swapColors(4, 1);
			break;
		default:
			break;
		}
	}

	void operator()(chtype c, int row, int col){
		//mvprintw();
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
	const arena_t startingArena_ = { //struct
		{ //vector
			{'d', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd'},
			{'d', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd'},
			{'d', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd'},
			{'d', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd'},
			{'d', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd'},
			{'d', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd'},
			{'d', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd'},
			{'d', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd'},
			{'d', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd'},
			{'d', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd'},
			{'d', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd'},
			{'d', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd'},
			{'d', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd'},
			{'d', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd'},
			{'d', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd'},
			{'d', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd'},
			{'d', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd'},
			{'d', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd'},
			{'d', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd'},
			{'d', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd'},
			{'d', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd'},
			{'d', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd'},
			{'d', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd'}
		} //vector
	}; //struct
	arena_t arena_ = startingArena_;
};

}

#endif /* GAME_INCLUDES_THREADS_ENGINE_THREADGAMEENGINE_HPP_ */
