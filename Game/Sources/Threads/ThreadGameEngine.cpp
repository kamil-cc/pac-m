/*
 * ThreadGameEngine.cpp
 *
 *  Created on: 8 wrz 2016
 *      Author: Kamil
 */

#include <Threads/Engine/ThreadGameEngine.hpp>

thd::GameEngine::GameEngine() {
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
	ghost1Direction_ = LEFT;
	ghost2Direction_ = UP;
	ghost3Direction_ = DOWN;
	ghost4Direction_ = RIGHT;
	sendRequest_ = false;
	gate_ = false;
}

void thd::GameEngine::printHelloInfo() {
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

void thd::GameEngine::printPoints() {
	static int cache = diamondsLeft_;
	mvprintw(14, GAME_COLS + 1, "        Points:");
	swapColors(1, 3);
	mvprintw(14, GAME_COLS + 1 + 16, "%d", (cache - diamondsLeft_));
	swapColors(3, 1);
}

void thd::GameEngine::printArena(const arena_t& arena) {
	for (int row = 0; row < static_cast<int>(arena.value.size()); ++row) {
		for (int col = 0; col < static_cast<int>(arena.value[0].size());
				++col) {
			auto bindedVisitor = boost::bind(*this, _1, row, col);
			boost::apply_visitor(bindedVisitor, arena.value[row][col]);
		}
		refresh();
	}
	refresh();
}

void thd::GameEngine::initEngine() {
	initscr();
	isNcursesMode = true;
	cbreak();
	noecho();
	timeout(0); //timeout(GAME_REFRESH_TIME.count()); //B³yskawicznie zwracaj getchar
	int maxRows = 0;
	int maxCols = 0;
	if (has_colors() == FALSE) {
		hasColorsFlag_ = false;
	} else {
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
	if ((maxRows == 0) || (maxRows < GAME_ROWS)) {
		gameAssert(!"Wrong rows num");
	}
	if ((maxCols == 0) || (maxCols < TOTAL_COLS)) {
		gameAssert(!"Wrong cols num");
	}
	if (resize_term(GAME_ROWS, TOTAL_COLS) == ERR) {
		gameAssert(!"resize_term failed");
	}
	printHelloInfo();
	printArena(startingArena_);
}

void thd::GameEngine::swapColors(int from, int to) {
	if (hasColorsFlag_) {
		attroff(COLOR_PAIR(from));
		attron(COLOR_PAIR(to));
	}
}

void thd::GameEngine::operator()(char c, int row, int col) { //Tworzy pola "specjalne"
	char ch = std::tolower(c);
	switch (ch) {
	case 'd':
		mvaddch(row, col, ACS_BULLET);
		break;
	case 'o':
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

void thd::GameEngine::operator()(chtype c, int row, int col) { //Tworzy pola "zwyk³e"
	if (((((c == ULC || c == LLC)) || (c == URC || c == LRC))
			|| ((c == HLI || c == VLI) || (c == TTE || c == BTE)))
			|| (c == LTE || c == RTE)) {
		swapColors(1, 3);
		mvaddch(row, col, c);
		swapColors(3, 1);
	}
}

void thd::GameEngine::printPressed(int key) {
	move(GAME_ROWS - 1, GAME_COLS + 1);
	clrtoeol();
	mvprintw(GAME_ROWS - 1, GAME_COLS + 1, "%d", key);
}

void thd::GameEngine::callQuit() {
	mtfifo::FIFODistributor& fifoDistributor =
			mtfifo::FIFODistributor::getInstance();
	mtfifo::FIFO<mtfifo::FIFOOutput> output1 = fifoDistributor.getFIFO<
			mtfifo::FIFOOutput>(mtfifo::FIFO_TCPIP_REC_EXIT);
	mtfifo::FIFO<mtfifo::FIFOOutput> output2 = fifoDistributor.getFIFO<
			mtfifo::FIFOOutput>(mtfifo::FIFO_TCPIP_SEN_EXIT);
	mtfifo::FIFO<mtfifo::FIFOOutput> output3 = fifoDistributor.getFIFO<
			mtfifo::FIFOOutput>(mtfifo::FIFO_LOG);
	boost::any exit = mtfifo::ExitThread();
	output1.put(exit); //Zamyka w¹tek odbieraj¹cy z sieci
	output2.put(exit); //Zamyka w¹tek pisz¹cy do sieci
	output3.put(exit); //Zamyka w¹tek pisz¹cy do loga
}

void thd::GameEngine::printGoodbyeScreen(bool win) {
	int row = 0;
	clear();
	timeout(-1);
	if (win) {
		swapColors(1, 2);
		mvprintw(++row, 0, "Winner!");
		swapColors(2, 1);
	} else {
		swapColors(1, 7);
		mvprintw(++row, 0, "Game over!");
		swapColors(7, 1);
	}
	mvprintw(++row, 0, "Thank You For Playing Pac-M!");
	mvprintw(++row, 0, "Author: Kamil Burzynski");
	mvprintw(++row, 0, "All Rights Reserved");
	mvprintw(++row, 0, "Press 'q' to quit.");
	int result;
	while ((result = getch()) != static_cast<int>('q')) {
		;
	}
}

void thd::GameEngine::moveGhost(int& row, int& col, bool& diamond,
		move_t& move) {
	static boost::random::mt19937 rng;
	static boost::random::uniform_int_distribution<> ten(1, 10);
	static boost::random::uniform_int_distribution<> four(0, 3);

	int up = 0;
	int left = 0;
	bool cachedDiamond = false;
	switch (move) {
	case UP: {
		up = -1;
		left = 0;
	}
		break;
	case DOWN: {
		up = 1;
		left = 0;
	}
		break;
	case LEFT: {
		up = 0;
		left = -1;
	}
		break;
	case RIGHT: {
		up = 0;
		left = 1;
	}
		break;
	}

	if ((((arena_.value[row + up][col + left] == n)
			|| (arena_.value[row + up][col + left] == d))
			|| (arena_.value[row + up][col + left] == O)) && (ten(rng) != 5)) {
		if (arena_.value[row + up][col + left] == d) {
			cachedDiamond = true;
		}
		if (arena_.value[row + up][col + left] == O) {
			lose_ = true;
			return;
		}
		arenaVariant_t cache = arena_.value[row][col];
		if (diamond) {
			arena_.value[row][col] = d;
			diamond = false;
		} else {
			arena_.value[row][col] = n;
		}
		auto localVisitor = boost::bind(*this, _1, row, col);
		boost::apply_visitor(localVisitor, arena_.value[row][col]);
		row += up;
		col += left;
		arena_.value[row][col] = cache;
		auto localVisitor1 = boost::bind(*this, _1, row, col);
		boost::apply_visitor(localVisitor1, arena_.value[row][col]);
		if (cachedDiamond) {
			diamond = cachedDiamond;
		}
		return;
	}

	int counter = 0;
	int randNum;
	int randomUp = 0;
	int randomLeft = 0;
	do {
		randNum = four(rng);
		switch (randNum) {
		case 0: {
			randomUp = -1;
			randomLeft = 0;
		}
			break;
		case 1: {
			randomUp = 1;
			randomLeft = 0;
		}
			break;
		case 2: {
			randomUp = 0;
			randomLeft = -1;
		}
			break;
		case 3: {
			randomUp = 0;
			randomLeft = 1;
		}
			break;
		}
		if (counter++ > 10) { //Zabezpieczenie przed zakleszczeniem
			return;
		}
	} while (!(((arena_.value[row + randomUp][col + randomLeft] == n)
			|| (arena_.value[row + randomUp][col + randomLeft] == d))
			|| (arena_.value[row + randomUp][col + randomLeft] == O)));

	if (arena_.value[row + randomUp][col + randomLeft] == d) {
		cachedDiamond = true;
	}
	if (arena_.value[row + up][col + left] == O) {
		lose_ = true;
		return;
	}
	arenaVariant_t cache = arena_.value[row][col];
	if (diamond) {
		arena_.value[row][col] = d;
		diamond = false;
	} else {
		arena_.value[row][col] = n;
	}
	auto localVisitor = boost::bind(*this, _1, row, col);
	boost::apply_visitor(localVisitor, arena_.value[row][col]);
	row += randomUp;
	col += randomLeft;
	arena_.value[row][col] = cache;
	auto localVisitor1 = boost::bind(*this, _1, row, col);
	boost::apply_visitor(localVisitor1, arena_.value[row][col]);
	if (cachedDiamond) {
		diamond = cachedDiamond;
	}
	if (randomLeft == 1)
		move = RIGHT;
	if (randomLeft == -1)
		move = LEFT;
	if (randomUp == 1)
		move = DOWN;
	if (randomUp == -1)
		move = UP;
	return;
}

void thd::GameEngine::movePacMan() {
	auto bindedVisitor = boost::bind(*this, _1, pacManRow_, pacManCol_);
	switch (readedChar_) {
	case 72: //Up
	{
		if (((arena_.value[pacManRow_ - 1][pacManCol_] == M)
				|| (arena_.value[pacManRow_ - 1][pacManCol_] == P))
				|| ((arena_.value[pacManRow_ - 1][pacManCol_] == S)
						|| (arena_.value[pacManRow_ - 1][pacManCol_] == T))) {
			lose_ = true;
		}
		if (arena_.value[pacManRow_ - 1][pacManCol_] == d) {
			--diamondsLeft_;
		}
		if ((((arena_.value[pacManRow_ - 1][pacManCol_] == n)
				|| (arena_.value[pacManRow_ - 1][pacManCol_] == d))
				|| ((arena_.value[pacManRow_ - 1][pacManCol_] == M)
						|| (arena_.value[pacManRow_ - 1][pacManCol_] == P)))
				|| ((arena_.value[pacManRow_ - 1][pacManCol_] == S)
						|| (arena_.value[pacManRow_ - 1][pacManCol_] == T))) {
			arena_.value[pacManRow_][pacManCol_] = n;
			boost::apply_visitor(bindedVisitor,
					arena_.value[pacManRow_][pacManCol_]);
			--pacManRow_;
			arena_.value[pacManRow_][pacManCol_] = O;
			auto localVisitor = boost::bind(*this, _1, pacManRow_, pacManCol_);
			boost::apply_visitor(localVisitor,
					arena_.value[pacManRow_][pacManCol_]);
		}
	}
		break;
	case 80: //Down
	{
		if (((arena_.value[pacManRow_ + 1][pacManCol_] == M)
				|| (arena_.value[pacManRow_ + 1][pacManCol_] == P))
				|| ((arena_.value[pacManRow_ + 1][pacManCol_] == S)
						|| (arena_.value[pacManRow_ + 1][pacManCol_] == T))) {
			lose_ = true;
		}
		if (arena_.value[pacManRow_ + 1][pacManCol_] == d) {
			--diamondsLeft_;
		}
		if ((((arena_.value[pacManRow_ + 1][pacManCol_] == n)
				|| (arena_.value[pacManRow_ + 1][pacManCol_] == d))
				|| ((arena_.value[pacManRow_ + 1][pacManCol_] == M)
						|| (arena_.value[pacManRow_ + 1][pacManCol_] == P)))
				|| ((arena_.value[pacManRow_ + 1][pacManCol_] == S)
						|| (arena_.value[pacManRow_ + 1][pacManCol_] == T))) {
			arena_.value[pacManRow_][pacManCol_] = n;
			boost::apply_visitor(bindedVisitor,
					arena_.value[pacManRow_][pacManCol_]);
			++pacManRow_;
			arena_.value[pacManRow_][pacManCol_] = O;
			auto localVisitor = boost::bind(*this, _1, pacManRow_, pacManCol_);
			boost::apply_visitor(localVisitor,
					arena_.value[pacManRow_][pacManCol_]);
		}
	}
		break;
	case 75: //Left
	{
		if (pacManCol_ == 0) { //Teleport
			arena_.value[pacManRow_][pacManCol_] = n;
			boost::apply_visitor(bindedVisitor,
					arena_.value[pacManRow_][pacManCol_]);
			pacManCol_ = GAME_COLS - 1;
			arena_.value[pacManRow_][pacManCol_] = O;
			auto localVisitor = boost::bind(*this, _1, pacManRow_, pacManCol_);
			boost::apply_visitor(localVisitor,
					arena_.value[pacManRow_][pacManCol_]);
		} else {
			if (((arena_.value[pacManRow_][pacManCol_ - 1] == M)
					|| (arena_.value[pacManRow_][pacManCol_ - 1] == P))
					|| ((arena_.value[pacManRow_][pacManCol_ - 1] == S)
							|| (arena_.value[pacManRow_][pacManCol_ - 1] == T))) {
				lose_ = true;
			}
			if (arena_.value[pacManRow_][pacManCol_ - 1] == d) {
				--diamondsLeft_;
			}
			if ((((arena_.value[pacManRow_][pacManCol_ - 1] == n)
					|| (arena_.value[pacManRow_][pacManCol_ - 1] == d))
					|| ((arena_.value[pacManRow_][pacManCol_ - 1] == M)
							|| (arena_.value[pacManRow_][pacManCol_ - 1] == P)))
					|| ((arena_.value[pacManRow_][pacManCol_ - 1] == S)
							|| (arena_.value[pacManRow_][pacManCol_ - 1] == T))) {
				arena_.value[pacManRow_][pacManCol_] = n;
				boost::apply_visitor(bindedVisitor,
						arena_.value[pacManRow_][pacManCol_]);
				--pacManCol_;
				arena_.value[pacManRow_][pacManCol_] = O;
				auto localVisitor = boost::bind(*this, _1, pacManRow_,
						pacManCol_);
				boost::apply_visitor(localVisitor,
						arena_.value[pacManRow_][pacManCol_]);
			}
		}
	}
		break;
	case 77: //Right
	{
		if (pacManCol_ == (GAME_COLS - 1)) { //Teleport
			arena_.value[pacManRow_][pacManCol_] = n;
			boost::apply_visitor(bindedVisitor,
					arena_.value[pacManRow_][pacManCol_]);
			pacManCol_ = 0;
			arena_.value[pacManRow_][pacManCol_] = O;
			auto localVisitor = boost::bind(*this, _1, pacManRow_, pacManCol_);
			boost::apply_visitor(localVisitor,
					arena_.value[pacManRow_][pacManCol_]);
		} else {
			if (((arena_.value[pacManRow_][pacManCol_ + 1] == M)
					|| (arena_.value[pacManRow_][pacManCol_ + 1] == P))
					|| ((arena_.value[pacManRow_][pacManCol_ + 1] == S)
							|| (arena_.value[pacManRow_][pacManCol_ + 1] == T))) {
				lose_ = true;
			}
			if (arena_.value[pacManRow_][pacManCol_ + 1] == d) {
				--diamondsLeft_;
			}
			if ((((arena_.value[pacManRow_][pacManCol_ + 1] == n)
					|| (arena_.value[pacManRow_][pacManCol_ + 1] == d))
					|| ((arena_.value[pacManRow_][pacManCol_ + 1] == M)
							|| (arena_.value[pacManRow_][pacManCol_ + 1] == P)))
					|| ((arena_.value[pacManRow_][pacManCol_ + 1] == S)
							|| (arena_.value[pacManRow_][pacManCol_ + 1] == T))) {
				arena_.value[pacManRow_][pacManCol_] = n;
				boost::apply_visitor(bindedVisitor,
						arena_.value[pacManRow_][pacManCol_]);
				++pacManCol_;
				arena_.value[pacManRow_][pacManCol_] = O;
				auto localVisitor = boost::bind(*this, _1, pacManRow_,
						pacManCol_);
				boost::apply_visitor(localVisitor,
						arena_.value[pacManRow_][pacManCol_]);
			}
		}
	}
		break;
	default:
		break;
	}
}

void thd::GameEngine::movePacMan(int row, int col) {
	arena_.value[pacManRow_][pacManCol_] = n;
	auto bindedVisitor1 = boost::bind(*this, _1, pacManRow_, pacManCol_);
	boost::apply_visitor(bindedVisitor1, arena_.value[pacManRow_][pacManCol_]);
	pacManRow_ = row;
	pacManCol_ = col;
	arena_.value[pacManRow_][pacManCol_] = O;
	auto localVisitor = boost::bind(*this, _1, pacManRow_, pacManCol_);
	boost::apply_visitor(localVisitor, arena_.value[pacManRow_][pacManCol_]);
}

void thd::GameEngine::moveGhost(int row, int col) {
	arena_.value[ghost1Row_][ghost1Col_] = n;
	auto bindedVisitor1 = boost::bind(*this, _1, ghost1Row_, ghost1Col_);
	boost::apply_visitor(bindedVisitor1, arena_.value[ghost1Row_][ghost1Col_]);
	ghost1Row_ = row;
	ghost1Col_ = col;
	arena_.value[ghost1Row_][ghost1Col_] = M;
	auto localVisitor = boost::bind(*this, _1, ghost1Row_, ghost1Col_);
	boost::apply_visitor(localVisitor, arena_.value[ghost1Row_][ghost1Col_]);
}

void thd::GameEngine::moveGhost(int up, int left, bool& diamond) {
	bool cachedDiamond = false;

	if (((arena_.value[ghost1Row_ + up][ghost1Col_ + left] == n)
			|| (arena_.value[ghost1Row_ + up][ghost1Col_ + left] == d))
			|| (arena_.value[ghost1Row_ + up][ghost1Col_ + left] == O)) {
		if (arena_.value[ghost1Row_ + up][ghost1Col_ + left] == d) {
			cachedDiamond = true;
		}
		if (arena_.value[ghost1Row_ + up][ghost1Col_ + left] == O) {
			lose_ = true;
			return;
		}
		arenaVariant_t cache = arena_.value[ghost1Row_][ghost1Col_];
		if (diamond) {
			arena_.value[ghost1Row_][ghost1Col_] = d;
			diamond = false;
		} else {
			arena_.value[ghost1Row_][ghost1Col_] = n;
		}
		auto localVisitor = boost::bind(*this, _1, ghost1Row_, ghost1Col_);
		boost::apply_visitor(localVisitor,
				arena_.value[ghost1Row_][ghost1Col_]);
		ghost1Row_ += up;
		ghost1Col_ += left;
		arena_.value[ghost1Row_][ghost1Col_] = cache;
		auto localVisitor1 = boost::bind(*this, _1, ghost1Row_, ghost1Col_);
		boost::apply_visitor(localVisitor1,
				arena_.value[ghost1Row_][ghost1Col_]);
		if (cachedDiamond) {
			diamond = cachedDiamond;
		}
		return;
	}
}

void thd::GameEngine::removeDiamond(int row, int col) {
	arena_.value[row][col] = n;
	auto localVisitor = boost::bind(*this, _1, row, col);
	boost::apply_visitor(localVisitor, arena_.value[row][col]);
}

//Punkt wejœcia w¹tku
void thd::GameEngine::operator()() {
	initEngine();

	mtfifo::FIFODistributor& fifoDistributor =
			mtfifo::FIFODistributor::getInstance();
	mtfifo::FIFO<mtfifo::FIFOOutput> log = fifoDistributor.getFIFO<
			mtfifo::FIFOOutput>(mtfifo::FIFO_LOG);
	mtfifo::FIFO<mtfifo::FIFOOutput> output = fifoDistributor.getFIFO<
			mtfifo::FIFOOutput>(mtfifo::FIFO_SERIALIZED_OUTPUT);
	mtfifo::FIFO<mtfifo::FIFOInput> input = fifoDistributor.getFIFO<
			mtfifo::FIFOInput>(mtfifo::FIFO_SERIALIZED_INPUT);

	boost::thread::id id = boost::this_thread::get_id();
	thd::ThreadRegistration& threadRegistration =
			thd::ThreadRegistration::getInstance();
	threadRegistration.registerThread(id, thd::ENGINE);

	gameStarted_ = false;
	singlePlayer_ = false;
	slave_ = false;
	win_ = false;
	lose_ = false;

	arena_.value = startingArena_.value; //Kopiowanie pola rozgrywki

	boost::posix_time::ptime startGame; //Czas startu gry
	//Czas do znikniêcia "bramy"
	boost::posix_time::time_duration ghostSleep = boost::posix_time::seconds(
			10);
	bool timeFlag = false;
	bool ghostsAreSleeping = true;

	while (1) {
		//TODO do loga: game is ready

		//Sekcja czytania z klawiatury
		readedChar_ = getch();
		if (readedChar_ != ERR)
			printPressed(readedChar_);
		if (readedChar_ == static_cast<int>('q')) {
			//TODO do loga program exited normally
			callQuit();
			break;
		}

		//Gra w trybie oczekiwania
		if (!gameStarted_) {
			if (readedChar_ == static_cast<int>('s')) {
				gameStarted_ = true;
				singlePlayer_ = true;
			} else if (readedChar_ == static_cast<int>('m')) {
				gameStarted_ = true;
				slave_ = true;
			} else {
				boost::any elem = input.get();
				try {
					boost::any_cast<boost::none_t>(elem);
				} catch (boost::bad_any_cast &e) {
					try {
						mtfifo::TCPIPSerialized serializedMsg = boost::any_cast<
								mtfifo::TCPIPSerialized>(elem);
						if (serializedMsg.serialized.compare("START")) {
							gameStarted_ = true;
						}
					} catch (boost::bad_any_cast &e) {
						gameAssert(!"Unknown element type");
					}
				}
			}
			boost::this_thread::sleep_for(
					boost::chrono::milliseconds(GAME_REFRESH_TIME));
			continue;
		}

		//Gra wystartowana
		if (!timeFlag) {
			startGame = boost::posix_time::second_clock::local_time();
			timeFlag = true;
		}
		printPoints();
		if (singlePlayer_) {
			if (timeFlag) {
				if ((boost::posix_time::second_clock::local_time() - startGame)
						> ghostSleep) {
					if (ghostsAreSleeping == true) {
						for (int i = 21; i <= 28; ++i) {
							arena_.value[10][i] = n;
							auto bindedVisitor = boost::bind(*this, _1, 10, i);
							boost::apply_visitor(bindedVisitor,
									arena_.value[10][i]);
						}
						ghostsAreSleeping = false;
					}
				}
			}

			moveGhost(ghost1Row_, ghost1Col_, ghost1Diamond_, ghost1Direction_);
			moveGhost(ghost2Row_, ghost2Col_, ghost2Diamond_, ghost2Direction_);
			moveGhost(ghost3Row_, ghost3Col_, ghost3Diamond_, ghost3Direction_);
			moveGhost(ghost4Row_, ghost4Col_, ghost4Diamond_, ghost4Direction_);

			movePacMan();

			if (!lose_)
				if (diamondsLeft_ == 0)
					win_ = true;
		} else {
			mvprintw(GAME_ROWS - 1, TOTAL_COLS - 2, "S"); //TODO usun¹c - debug print
			refresh();
			if (slave_) {
				//TODO instrukcje do gry sieciowej
				if (!sendRequest_) {
					std::string startMsg = "START";
					boost::any statrReq = mtfifo::TCPIPSerialized(startMsg);
					output.put(statrReq);
					sendRequest_ = true;
				}
				std::string message = "SLAVE ";
				switch (readedChar_) { //Gdy odczytano niew³aœciwy klawisz, nie rób nic
				case 72: //Up
				{
					message += "UP";
					boost::any msgToSend = mtfifo::TCPIPSerialized(message); //Heartbeat
					output.put(msgToSend);
				}
					break;
				case 80: //Down
				{
					message += "DOWN";
					boost::any msgToSend = mtfifo::TCPIPSerialized(message); //Heartbeat
					output.put(msgToSend);
				}
					break;
				case 75: //Left
				{
					message += "LEFT";
					boost::any msgToSend = mtfifo::TCPIPSerialized(message); //Heartbeat
					output.put(msgToSend);
				}
					break;
				case 77: //Right
				{
					message += "RIGHT";
					boost::any msgToSend = mtfifo::TCPIPSerialized(message); //Heartbeat
					output.put(msgToSend);
				}
					break;
				}

				boost::any msgReceived = input.get();
				try {
					boost::any_cast<boost::none_t>(msgReceived);
				} catch (boost::bad_any_cast &e) {
					try {
						mtfifo::TCPIPSerialized serialized = boost::any_cast<
								mtfifo::TCPIPSerialized>(msgReceived);
						std::string command = serialized.serialized;

						std::vector<std::string> tokens;
						boost::split(tokens, command, boost::is_any_of(" "));

						if (tokens.size() < 2) {
							boost::this_thread::sleep_for(
									boost::chrono::milliseconds(
											GAME_REFRESH_TIME));
							continue;
						}

						if(tokens[0].find("SLAVE") != std::string::npos){
							if(tokens[1].find("UP") != std::string::npos){
								moveGhost(-1, 0, ghost1Diamond_);
							}
							if(tokens[1].find("DOWN") != std::string::npos){
								moveGhost(1, 0, ghost1Diamond_);
							}
							if(tokens[1].find("LEFT") != std::string::npos){
								moveGhost(0, -1, ghost1Diamond_);
							}
							if(tokens[1].find("RIGHT") != std::string::npos){
								moveGhost(0, 1, ghost1Diamond_);
							}
						}

						if (tokens.size() < 3) {
							boost::this_thread::sleep_for(
									boost::chrono::milliseconds(
											GAME_REFRESH_TIME));
							continue;
						}

						if (tokens[0].find("MASTER") != std::string::npos) {
							movePacMan(boost::lexical_cast<int>(tokens[1]),
									boost::lexical_cast<int>(tokens[2]));
						}
						if (tokens[0].find("DIAMOND") != std::string::npos) {
							removeDiamond(boost::lexical_cast<int>(tokens[1]),
									boost::lexical_cast<int>(tokens[2]));
						}
					} catch (boost::bad_any_cast &e) {
						gameAssert(!"Wrong element type");
					}
				}
			} else {
				mvprintw(GAME_ROWS - 1, TOTAL_COLS - 2, "M"); //TODO usun¹c - debug print
				refresh();
				//Gra znajduje siê w trybie master
				if (!gate_) {
					for (int i = 21; i <= 28; ++i) {
						removeDiamond(10, i);
						std::string clearString = "DIAMOND "
								+ boost::lexical_cast<std::string>(10) + " "
								+ boost::lexical_cast<std::string>(i);
						boost::any clear = mtfifo::TCPIPSerialized(clearString);
						output.put(clear);
					}
					gate_ = true;
				}

				movePacMan();
				std::string masterCommand = "MASTER ";
				masterCommand += boost::lexical_cast<std::string>(pacManRow_)
						+ " " + boost::lexical_cast<std::string>(pacManCol_);
				boost::any masterMove = mtfifo::TCPIPSerialized(masterCommand);
				output.put(masterMove);

				boost::any slaveMove = input.get();
				try {
					boost::any_cast<boost::none_t>(slaveMove);
				} catch (boost::bad_any_cast &e) {
					try {
						mtfifo::TCPIPSerialized slaveCommand = boost::any_cast<
								mtfifo::TCPIPSerialized>(slaveMove);

						std::string slaveString = slaveCommand.serialized;

						std::vector<std::string> tokens1;
						boost::split(tokens1, slaveString,
								boost::is_any_of(" "));

						log.put(
								boost::any(
										mtfifo::LogElement(slaveString + " SS",
												critical,
												boost::this_thread::get_id())));

						if (tokens1.size() < 2) {
							boost::this_thread::sleep_for(
									boost::chrono::milliseconds(
											GAME_REFRESH_TIME));
							continue;
						}

						if (tokens1[0].find("SLAVE") != std::string::npos) {

							if (tokens1[1].find("UP") != std::string::npos) {
								moveGhost(-1, 0, ghost1Diamond_);
							}

							if (tokens1[1].find("DOWN") != std::string::npos) {
								moveGhost(1, 0, ghost1Diamond_);
							}

							if (tokens1[1].find("LEFT") != std::string::npos) {
								moveGhost(0, -1, ghost1Diamond_);
							}

							if (tokens1[1].find("RIGHT") != std::string::npos) {
								moveGhost(0, 1, ghost1Diamond_);
							}

							std::string slaveEcho = slaveString;
							boost::any echo = mtfifo::TCPIPSerialized(slaveEcho);
							output.put(echo);
						}
					} catch (boost::bad_any_cast &e) {
						gameAssert(!"Recived bad type");
					}
				}
			}
			refresh();
		}

		//Gra zakoñczona
		if (win_ || lose_) {
			printGoodbyeScreen(win_);
			callQuit();
			break;
		}
		boost::this_thread::sleep_for(
				boost::chrono::milliseconds(GAME_REFRESH_TIME));
	}
	endwin();
	isNcursesMode = false;
}
