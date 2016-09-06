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

typedef enum move_e{UP, DOWN, LEFT, RIGHT, NONE} move_t;
typedef enum creature_e{GHOST1, GHOST2, GHOST4, PACMAN, LAST} creature_t;
typedef boost::variant<creature_t> creatureVariant_t;

//Arena sk³ada siê z dwóch typów
typedef boost::variant<char, chtype> arenaVariant_t;

//Na wzór boost::array
typedef struct arena_s{std::vector<std::vector<arenaVariant_t> > value;} arena_t;

class GameEngine : public boost::static_visitor<>{ // : boost::static_visitor<void>
public:
	void printHelloInfo(){
		const int ANCHOR = 12;
		int row = 0;
		mvprintw(++row, GAME_COLS + 1, "         Hello!");
		mvprintw(++row, GAME_COLS + 1, "For Single Player press ' '");
		swapColors(1, 2);
		mvprintw(row, GAME_COLS + 1 + 25, "s");
		swapColors(2, 1);
		mvprintw(++row, GAME_COLS + 1, "For Multi Player press ' '");
		swapColors(1, 2);
		mvprintw(row, GAME_COLS + 1 + 24, "m");
		swapColors(2, 1);
		mvprintw(++row, GAME_COLS + 1, "To exit game press ' '");
		swapColors(1, 2);
		mvprintw(row, GAME_COLS + 1 + 20, "q");
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
		noecho();
		timeout(GAME_REFRESH_TIME.count()); //B³yskawicznie zwracaj getchar
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
		diamondsLeft_ = 0;
		printArena();
	}

	void swapColors(int from, int to){
		if(hasColorsFlag_){
			attroff(COLOR_PAIR(from));
			attron(COLOR_PAIR(to));
		}
	}

	void operator()(char c, int row, int col){ //Tworzy pola "specjalne"
		char ch = std::tolower(c);
		switch(ch){
		case'd':
			++diamondsLeft_;
			mvaddch(row, col, ACS_BULLET);
			break;
		case'o':
			swapColors(1, 4);
			mvaddch(row, col, 'O');
			swapColors(4, 1);
			pacManRow_ = row;
			pacManCol_ = col;
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
			ghost1Row_ = row;
			ghost1Col_ = col;
			break;
		case 'p':
			swapColors(1, 6);
			mvaddch(row, col, 'M');
			swapColors(6, 1);
			ghost2Row_ = row;
			ghost2Col_ = col;
			break;
		case 's':
			swapColors(1, 6);
			mvaddch(row, col, 'M');
			swapColors(6, 1);
			ghost3Row_ = row;
			ghost3Col_ = col;
			break;
		case 't':
			swapColors(1, 6);
			mvaddch(row, col, 'M');
			swapColors(6, 1);
			ghost4Row_ = row;
			ghost4Col_ = col;
			break;
		default:
			break;
		}
	}

	void operator()(chtype c, int row, int col){ //Tworzy pola "zwyk³e"
		if(((((c == ULC || c == LLC)) || (c == URC || c == LRC))
			|| ((c == HLI || c == VLI) || (c == TTE || c == BTE)))
			|| (c == LTE || c == RTE)){
				swapColors(1, 3);
				mvaddch(row, col, c);
				swapColors(3, 1);
		}
	}

	void operator()(creature_t creature, move_t move){ //Operator przesuwaj¹cy potworki
		switch(creature){
		case PACMAN:
			if(move == NONE)
				break;
			break;
		default:
			break;
		}
	}

	void printPressed(int key){
		move(GAME_ROWS - 1, GAME_COLS + 1);
		clrtoeol();
		mvprintw(GAME_ROWS - 1, GAME_COLS + 1, "%d", key);
	}

	void callQuit(){
		mtfifo::FIFODistributor& fifoDistributor = mtfifo::FIFODistributor::getInstance();
		mtfifo::FIFO<mtfifo::FIFOOutput> output1
					= fifoDistributor.getFIFO<mtfifo::FIFOOutput>(mtfifo::FIFO_TCPIP_REC_EXIT);
		mtfifo::FIFO<mtfifo::FIFOOutput> output2
					= fifoDistributor.getFIFO<mtfifo::FIFOOutput>(mtfifo::FIFO_TCPIP_SEN_EXIT);
		mtfifo::FIFO<mtfifo::FIFOOutput> output3
					= fifoDistributor.getFIFO<mtfifo::FIFOOutput>(mtfifo::FIFO_LOG);
		boost::any exit = mtfifo::ExitThread();
		output1.put(exit); //Zamyka w¹tek odbieraj¹cy z sieci
		output2.put(exit); //Zamyka w¹tek pisz¹cy do sieci
		output3.put(exit); //Zamyka w¹tek pisz¹cy do loga
	}

	void printGoodbyeScreen(bool win){
		int row = 0;
		clear();
		timeout(-1);
		if(win){
			swapColors(1, 2);
			mvprintw(++row, 0, "Winner!");
			swapColors(2, 1);
		}else{
			swapColors(1, 7);
			mvprintw(++row, 0, "Game over!");
			swapColors(7, 1);
		}
		mvprintw(++row, 0, "Thank You For Playing Pac-M!");
		mvprintw(++row, 0, "Author: Kamil Burzynski");
		mvprintw(++row, 0, "All Rights Reserved");
		mvprintw(++row, 0, "Press any key to quit.");
		getch();
	}

	//Punkt wejœcia w¹tku
	void operator()(){
		initEngine();

		mtfifo::FIFODistributor& fifoDistributor = mtfifo::FIFODistributor::getInstance();
		mtfifo::FIFO<mtfifo::FIFOOutput> log
			= fifoDistributor.getFIFO<mtfifo::FIFOOutput>(mtfifo::FIFO_LOG);
		mtfifo::FIFO<mtfifo::FIFOOutput> output
					= fifoDistributor.getFIFO<mtfifo::FIFOOutput>(mtfifo::FIFO_SERIALIZED_OUTPUT);
		mtfifo::FIFO<mtfifo::FIFOInput> input
					= fifoDistributor.getFIFO<mtfifo::FIFOInput>(mtfifo::FIFO_SERIALIZED_INPUT);

		boost::thread::id id = boost::this_thread::get_id();
		thd::ThreadRegistration& threadRegistration = thd::ThreadRegistration::getInstance();
		threadRegistration.registerThread(id, thd::ENGINE);

		gameStarted_ = false;
		singlePlayer_ = false;
		slave_ = false;
		win_ = false;
		lose_ = false;

		while(1){
			//TODO do loga: game is ready

			//Sekcja czytania z klawiatury
			readedChar_ = getch();
			if(readedChar_ != ERR)
				printPressed(readedChar_);
			if(readedChar_ == static_cast<int>('q')){
				//TODO do loga program exited normally
				callQuit();
				break;
			}

			//Gra w trybie oczekiwania
			if(!gameStarted_){
				if(readedChar_ == static_cast<int>('s')){
					gameStarted_ = true;
					singlePlayer_ = true;
				}else if(readedChar_ == static_cast<int>('m')){
					gameStarted_ = true;
					slave_ = true;
				}else{
					boost::any elem = input.get();
		        	try{
						boost::any_cast<boost::none_t>(elem);
					}catch(boost::bad_any_cast &e){
						try{
							mtfifo::TCPIPSerialized serializedMsg =
									boost::any_cast<mtfifo::TCPIPSerialized>(elem);
							if(serializedMsg.serialized.compare("START")){
								gameStarted_ = true;
							}
						}catch(boost::bad_any_cast &e){
							assert(!"Unknown element type");
						}
					}
				}
				boost::this_thread::sleep_for(boost::chrono::milliseconds(GAME_REFRESH_TIME));
				continue;
			}

			if(readedChar_ == 72){
				nextMove_ = UP;
			}else if(readedChar_ == 80){
				nextMove_ = DOWN;
			}else if(readedChar_ == 75){
				nextMove_ = LEFT;
			}else if(readedChar_ == 77){
				nextMove_ = RIGHT;
			}else{
				nextMove_ = NONE;
			}

			//Gra wystartowana
			if(singlePlayer_){
				for(int i = GHOST1; i != LAST; ++i){
					move_t move;
					creature_t creature = static_cast<creature_t>(i);
					creatureVariant_t creatureToVisit = creature;
					if(creature == PACMAN){
						move = nextMove_;
					}else{
						move = NONE;
					}
					auto creatureVisitor = boost::bind(*this, _1, move);
					boost::apply_visitor(creatureVisitor, creatureToVisit);
				}
			}else{
				//TODO instrukcje do gry sieciowej
				//Lub pobranie z kolejki ruchu duszka analogicznie jw + pobranie ruchu duszka
			}

			//Gra zakoñczona
			if(win_ || lose_){
				printGoodbyeScreen(win_);
				callQuit();
				break;
			}
			boost::this_thread::sleep_for(boost::chrono::milliseconds(GAME_REFRESH_TIME));
		}
		endwin();
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
	arena_t arena_ = startingArena_;
	int diamondsLeft_;
	int readedChar_;
	bool gameStarted_;
	bool singlePlayer_;
	bool slave_;
	bool win_;
	bool lose_;
	//Po³o¿enie PacMana
	int pacManRow_;
	int pacManCol_;
	//Po³o¿enie Duchów
	int ghost1Row_;
	int ghost1Col_;
	int ghost2Row_;
	int ghost2Col_;
	int ghost3Row_;
	int ghost3Col_;
	int ghost4Row_;
	int ghost4Col_;
	//Next move
	move_t nextMove_;
};

}

#endif /* GAME_INCLUDES_THREADS_ENGINE_THREADGAMEENGINE_HPP_ */
