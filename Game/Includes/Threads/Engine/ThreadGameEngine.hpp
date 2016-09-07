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

namespace thd{

const int GAME_ROWS = 25; //TODO do sk³adowych statycznych klasy GameEngine
const int GAME_COLS = 50;
const int INFO_COLS = 30;
const int TOTAL_COLS = GAME_COLS + INFO_COLS;

typedef enum move_e{UP, DOWN, LEFT, RIGHT, NONE} move_t;
typedef enum creature_e{PACMAN, GHOST1, GHOST2, GHOST4, LAST} creature_t;
typedef boost::variant<creature_t> creatureVariant_t;

//Arena sk³ada siê z dwóch typów
typedef boost::variant<char, chtype> arenaVariant_t;
//Zmienne do porównywania wartoœci
arenaVariant_t d = 'd';
arenaVariant_t n = ' ';
arenaVariant_t M = 'M';
arenaVariant_t P = 'P';
arenaVariant_t S = 'S';
arenaVariant_t T = 'T';
arenaVariant_t O = 'O';

//Na wzór boost::array
typedef struct arena_s{std::vector<std::vector<arenaVariant_t> > value;} arena_t;

class GameEngine : public boost::static_visitor<>{ // : boost::static_visitor<void>
public:
	GameEngine(){
		ghost1Row_ = 12;
		ghost1Col_ = 21;
		ghost2Row_ = 12;
		ghost2Col_ = 23;
		ghost3Row_ = 12;
		ghost3Col_ = 26;
		ghost4Row_ = 12;
		ghost4Col_ = 28;
		pacManRow_ = 15;
		pacManCol_ = 24;
		diamondsLeft_ = 421;
		ghost1Diamond_ = false;
		ghost2Diamond_ = false;
		ghost3Diamond_ = false;
		ghost4Diamond_ = false;
		g1_ = LEFT;
		g2_ = UP;
		g3_ = DOWN;
		g4_ = RIGHT;
	}

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
		refresh();
	}

	void printPoints(){
		static int cache = diamondsLeft_;
		mvprintw(14, GAME_COLS + 1, "        Points:");
		swapColors(1, 3);
		mvprintw(14, GAME_COLS + 1 + 16, "%d", (cache - diamondsLeft_));
		swapColors(3, 1);
	}

	void printArena(const arena_t& arena){
		for(int row = 0; row < static_cast<int>(arena.value.size()); ++row){
			for(int col = 0; col < static_cast<int>(arena.value[0].size()); ++col){
				auto bindedVisitor = boost::bind(*this, _1, row, col);
				boost::apply_visitor(bindedVisitor, arena.value[row][col]);
			}
			refresh();
		}
		refresh();
	}

	void initEngine(){
		initscr();
		cbreak();
		noecho();
		timeout(0);//timeout(GAME_REFRESH_TIME.count()); //B³yskawicznie zwracaj getchar
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
		printArena(startingArena_);
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
		case 'p':
			swapColors(1, 6);
			mvaddch(row, col, 'M');
			swapColors(6, 1);
			break;
		case 's':
			swapColors(1, 6);
			mvaddch(row, col, 'M');
			swapColors(6, 1);
			break;
		case 't':
			swapColors(1, 6);
			mvaddch(row, col, 'M');
			swapColors(6, 1);
			break;
		case ' ':
			mvaddch(row, col, ' ');
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

	void moveGhost(int& row, int& col, bool& diamond, move_t& move){
		static boost::random::mt19937 rng;
		static boost::random::uniform_int_distribution<> ten(1,10);
		static boost::random::uniform_int_distribution<> four(0, 3);

		int up = 0;
		int left = 0;
		bool cachedDiamond = false;
		switch(move){
				case UP:
				{
					up = -1;
					left = 0;
				}
					break;
				case DOWN:
				{
					up = 1;
					left = 0;
				}
					break;
				case LEFT:
				{
					up = 0;
					left = -1;
				}
					break;
				case RIGHT:
				{
					up = 0;
					left = 1;
				}
					break;
		}

		if((((arena_.value[row + up][col + left] == n)
						|| (arena_.value[row + up][col + left] == d))
						|| (arena_.value[row + up][col + left] == O))
						&& (ten(rng) != 5)){
			if(arena_.value[row + up][col + left] == d){
				cachedDiamond = true;
			}
			if(arena_.value[row + up][col + left] == O){
				lose_ = true;
				return;
			}
			arenaVariant_t cache = arena_.value[row][col];
			if(diamond){
				arena_.value[row][col] = d;
				diamond = false;
			}else{
				arena_.value[row][col] = n;
			}
			auto localVisitor = boost::bind(*this, _1, row, col);
			boost::apply_visitor(localVisitor, arena_.value[row][col]);
			row += up;
			col += left;
			arena_.value[row][col] = cache;
			auto localVisitor1 = boost::bind(*this, _1, row, col);
			boost::apply_visitor(localVisitor1, arena_.value[row][col]);
			if(cachedDiamond){
				diamond = cachedDiamond;
			}
			return;
		}

		int counter = 0;
		int randNum;
		int randomUp = 0;
		int randomLeft = 0;
		do{
				randNum = four(rng);
				switch(randNum){
				case 0:
				{
					randomUp = -1;
					randomLeft = 0;
				}
					break;
				case 1:
				{
					randomUp = 1;
					randomLeft = 0;
				}
					break;
				case 2:
				{
					randomUp = 0;
					randomLeft = -1;
				}
					break;
				case 3:
				{
					randomUp = 0;
					randomLeft = 1;
				}
					break;
			}
			if(counter++ > 10){ //Zabezpieczenie przed zakleszczeniem
				return;
			}
		}while(!(((arena_.value[row + randomUp][col + randomLeft] == n)
				|| (arena_.value[row + randomUp][col + randomLeft] == d))
				|| (arena_.value[row + randomUp][col + randomLeft] == O)));

		if(arena_.value[row + randomUp][col + randomLeft] == d){
			cachedDiamond = true;
		}
		if(arena_.value[row + up][col + left] == O){
			lose_ = true;
			return;
		}
		arenaVariant_t cache = arena_.value[row][col];
		if(diamond){
			arena_.value[row][col] = d;
			diamond = false;
		}else{
			arena_.value[row][col] = n;
		}
		auto localVisitor = boost::bind(*this, _1, row, col);
		boost::apply_visitor(localVisitor, arena_.value[row][col]);
		row += randomUp;
		col += randomLeft;
		arena_.value[row][col] = cache;
		auto localVisitor1 = boost::bind(*this, _1, row, col);
		boost::apply_visitor(localVisitor1, arena_.value[row][col]);
		if(cachedDiamond){
			diamond = cachedDiamond;
		}
		if(randomLeft == 1)
			move = RIGHT;
		if(randomLeft == -1)
			move = LEFT;
		if(randomUp == 1)
			move = DOWN;
		if(randomUp == -1)
			move = UP;
		return;
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

		arena_.value = startingArena_.value; //Kopiowanie pola rozgrywki

	    boost::posix_time::ptime startGame;
	    boost::posix_time::time_duration ghostSleep = boost::posix_time::seconds(10);
	    bool timeFlag = false;
	    bool ghostsAreSleeping = true;

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

			//Gra wystartowana
			if(!timeFlag){
				startGame = boost::posix_time::second_clock::local_time();
				timeFlag = true;
			}
			printPoints();
			if(singlePlayer_){
				if(timeFlag){
					if((boost::posix_time::second_clock::local_time() - startGame) > ghostSleep){
						if(ghostsAreSleeping == true){
							for(int i = 21; i <= 28; ++i){
								arena_.value[10][i] = n;
								auto bindedVisitor = boost::bind(*this, _1, 10, i);
								boost::apply_visitor(bindedVisitor, arena_.value[10][i]);
							}
							ghostsAreSleeping = false;
						}
					}
				}

				moveGhost(ghost1Row_, ghost1Col_, ghost1Diamond_, g1_);
				moveGhost(ghost2Row_, ghost2Col_, ghost2Diamond_, g2_);
				moveGhost(ghost3Row_, ghost3Col_, ghost3Diamond_, g3_);
				moveGhost(ghost4Row_, ghost4Col_, ghost4Diamond_, g4_);

				auto bindedVisitor = boost::bind(*this, _1, pacManRow_, pacManCol_);
				switch(readedChar_){
				case 72: //Up
				{
					if(((arena_.value[pacManRow_ - 1][pacManCol_] == M)
										|| (arena_.value[pacManRow_ - 1][pacManCol_] == P))
										|| ((arena_.value[pacManRow_ - 1][pacManCol_] == S)
										|| (arena_.value[pacManRow_ - 1][pacManCol_] == T))){
						lose_ = true;
					}
					if(arena_.value[pacManRow_ - 1][pacManCol_] == d){
						--diamondsLeft_;
					}
					if((((arena_.value[pacManRow_ - 1][pacManCol_] == n)
					|| (arena_.value[pacManRow_ - 1][pacManCol_] == d))
					|| ((arena_.value[pacManRow_ - 1][pacManCol_] == M)
					|| (arena_.value[pacManRow_ - 1][pacManCol_] == P)))
					|| ((arena_.value[pacManRow_ - 1][pacManCol_] == S)
					|| (arena_.value[pacManRow_ - 1][pacManCol_] == T))){
						arena_.value[pacManRow_][pacManCol_] = n;
						boost::apply_visitor(bindedVisitor, arena_.value[pacManRow_][pacManCol_]);
						--pacManRow_;
						arena_.value[pacManRow_][pacManCol_] = O;
						auto localVisitor = boost::bind(*this, _1, pacManRow_, pacManCol_);
						boost::apply_visitor(localVisitor, arena_.value[pacManRow_][pacManCol_]);
					}
				}
					break;
				case 80: //Down
				{
					if(((arena_.value[pacManRow_ + 1][pacManCol_] == M)
							|| (arena_.value[pacManRow_ + 1][pacManCol_] == P))
							|| ((arena_.value[pacManRow_ + 1][pacManCol_] == S)
							|| (arena_.value[pacManRow_ + 1][pacManCol_] == T))){
						lose_ = true;
					}
					if(arena_.value[pacManRow_ + 1][pacManCol_] == d){
						--diamondsLeft_;
					}
					if((((arena_.value[pacManRow_ + 1][pacManCol_] == n)
					|| (arena_.value[pacManRow_ + 1][pacManCol_] == d))
					|| ((arena_.value[pacManRow_ + 1][pacManCol_] == M)
					|| (arena_.value[pacManRow_ + 1][pacManCol_] == P)))
					|| ((arena_.value[pacManRow_ + 1][pacManCol_] == S)
					|| (arena_.value[pacManRow_ + 1][pacManCol_] == T))){
						arena_.value[pacManRow_][pacManCol_] = n;
						boost::apply_visitor(bindedVisitor, arena_.value[pacManRow_][pacManCol_]);
						++pacManRow_;
						arena_.value[pacManRow_][pacManCol_] = O;
						auto localVisitor = boost::bind(*this, _1, pacManRow_, pacManCol_);
						boost::apply_visitor(localVisitor, arena_.value[pacManRow_][pacManCol_]);
					}
				}
					break;
				case 75: //Left
				{
					if(pacManCol_ == 0){//Teleport
						arena_.value[pacManRow_][pacManCol_] = n;
						boost::apply_visitor(bindedVisitor, arena_.value[pacManRow_][pacManCol_]);
						pacManCol_ = GAME_COLS - 1;
						arena_.value[pacManRow_][pacManCol_] = O;
						auto localVisitor = boost::bind(*this, _1, pacManRow_, pacManCol_);
						boost::apply_visitor(localVisitor, arena_.value[pacManRow_][pacManCol_]);
					}else{
						if(((arena_.value[pacManRow_][pacManCol_ - 1] == M)
								|| (arena_.value[pacManRow_][pacManCol_ - 1] == P))
								|| ((arena_.value[pacManRow_][pacManCol_ - 1] == S)
								|| (arena_.value[pacManRow_][pacManCol_ - 1] == T))){
							lose_ = true;
						}
						if(arena_.value[pacManRow_][pacManCol_ - 1] == d){
							--diamondsLeft_;
						}
						if((((arena_.value[pacManRow_][pacManCol_ - 1] == n)
						|| (arena_.value[pacManRow_][pacManCol_ - 1] == d))
						|| ((arena_.value[pacManRow_][pacManCol_ - 1] == M)
						|| (arena_.value[pacManRow_][pacManCol_ - 1] == P)))
						|| ((arena_.value[pacManRow_][pacManCol_ - 1] == S)
						|| (arena_.value[pacManRow_][pacManCol_ - 1] == T))){
							arena_.value[pacManRow_][pacManCol_] = n;
							boost::apply_visitor(bindedVisitor, arena_.value[pacManRow_][pacManCol_]);
							--pacManCol_;
							arena_.value[pacManRow_][pacManCol_] = O;
							auto localVisitor = boost::bind(*this, _1, pacManRow_, pacManCol_);
							boost::apply_visitor(localVisitor, arena_.value[pacManRow_][pacManCol_]);
						}
					}
				}
					break;
				case 77: //Right
				{
					if(pacManCol_ == (GAME_COLS - 1)){ //Teleport
						arena_.value[pacManRow_][pacManCol_] = n;
						boost::apply_visitor(bindedVisitor, arena_.value[pacManRow_][pacManCol_]);
						pacManCol_ = 0;
						arena_.value[pacManRow_][pacManCol_] = O;
						auto localVisitor = boost::bind(*this, _1, pacManRow_, pacManCol_);
						boost::apply_visitor(localVisitor, arena_.value[pacManRow_][pacManCol_]);
					}else{
						if(((arena_.value[pacManRow_][pacManCol_ + 1] == M)
								|| (arena_.value[pacManRow_][pacManCol_ + 1] == P))
								|| ((arena_.value[pacManRow_][pacManCol_ + 1] == S)
								|| (arena_.value[pacManRow_][pacManCol_ + 1] == T))){
							lose_ = true;
						}
						if(arena_.value[pacManRow_][pacManCol_ + 1] == d){
							--diamondsLeft_;
						}
						if((((arena_.value[pacManRow_][pacManCol_ + 1] == n)
						|| (arena_.value[pacManRow_][pacManCol_ + 1] == d))
						|| ((arena_.value[pacManRow_][pacManCol_ + 1] == M)
						|| (arena_.value[pacManRow_][pacManCol_ + 1] == P)))
						|| ((arena_.value[pacManRow_][pacManCol_ + 1] == S)
						|| (arena_.value[pacManRow_][pacManCol_ + 1] == T))){
							arena_.value[pacManRow_][pacManCol_] = n;
							boost::apply_visitor(bindedVisitor, arena_.value[pacManRow_][pacManCol_]);
							++pacManCol_;
							arena_.value[pacManRow_][pacManCol_] = O;
							auto localVisitor = boost::bind(*this, _1, pacManRow_, pacManCol_);
							boost::apply_visitor(localVisitor, arena_.value[pacManRow_][pacManCol_]);
						}
					}
				}
					break;
				default:
					break;
				}
				if(!lose_)
					if(diamondsLeft_ == 0)
						win_ = true;
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
	move_t g1_;
	move_t g2_;
	move_t g3_;
	move_t g4_;
};

}

#endif /* GAME_INCLUDES_THREADS_ENGINE_THREADGAMEENGINE_HPP_ */
