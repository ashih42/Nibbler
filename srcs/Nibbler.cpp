#include "Nibbler.hpp"
#include "NibblerException.hpp"
#include "Board.hpp"
#include "IModule.hpp"
#include "SDLModule.hpp"
#include "SFMLModule.hpp"
#include "OpenGLModule.hpp"
#include "Snake.hpp"
#include "AudioManager.hpp"
#include "Enemy.hpp"
#include "Server.hpp"
#include "Client.hpp"

#include <chrono>

/*
	Static variables and methods
*/

Nibbler *					Nibbler::_instance = nullptr;

void						Nibbler::createLocalGame(int boardWidth, int boardHeight, int numPlayers)
{
	Nibbler::_instance = new Nibbler(boardWidth, boardHeight, numPlayers);
	Nibbler::_instance->_loop();

	delete Nibbler::_instance;
	Nibbler::_instance = nullptr;
}

void						Nibbler::createOnlineGameAsServer(int boardWidth, int boardHeight, unsigned short port)
{
	Nibbler::_instance = new Nibbler(boardWidth, boardHeight, port);
	Nibbler::_instance->_loop();

	delete Nibbler::_instance;
	Nibbler::_instance = nullptr;
}

void						Nibbler::createOnlineGameAsClient(std::string & ipAddress, unsigned short port)
{
	Nibbler::_instance = new Nibbler(ipAddress, port);
	Nibbler::_instance->_loop();

	delete Nibbler::_instance;
	Nibbler::_instance = nullptr;
}

Nibbler &					Nibbler::getInstance(void)
{
	if (Nibbler::_instance == nullptr)
		throw NibblerException("Nibbler::_instance is null");

	return (*Nibbler::_instance);
}

/*
	Instance methods
*/

/* Local Game Constructor */
Nibbler::Nibbler(int boardWidth, int boardHeight, int numPlayers) :
	_numPlayers(numPlayers),
	_exitProgram(false),
	_isRoundOver(false),
	_roundNumber(0)
{
	std::srand(std::time(0));
	this->_validateArgs(boardWidth, boardHeight, numPlayers);

	this->_server = nullptr;
	this->_client = nullptr;
	
	this->_board = new Board(boardWidth, boardHeight);
	this->_snakes.resize(this->_numPlayers);
	this->_initModules();
	this->_startNewRound();
}

/* Online 2P Server Constructor */
Nibbler::Nibbler(int boardWidth, int boardHeight, unsigned short port) :
	_numPlayers(2),
	_exitProgram(false),
	_isRoundOver(false),
	_roundNumber(0)
{
	std::srand(std::time(0));
	this->_validateArgs(boardWidth, boardHeight, 2);

	this->_server = new Server(port);
	this->_client = nullptr;

	this->_board = new Board(boardWidth, boardHeight);
	this->_server->sendBoardInfo(*this->_board);

	this->_snakes.resize(this->_numPlayers);
	this->_initModules();
	this->_startNewRound();
}

/* Online 2P Client Constructor */
Nibbler::Nibbler(std::string & ipAddress, unsigned short port) :
	_numPlayers(2),
	_exitProgram(false),
	_isRoundOver(false),
	_roundNumber(0)
{
	int						boardWidth;
	int						boardHeight;

	std::srand(std::time(0));

	this->_server = nullptr;
	this->_client = new Client(ipAddress, port);
	
	this->_client->receiveBoardInfo(boardWidth, boardHeight);
	this->_board = new Board(boardWidth, boardHeight);

	this->_snakes.resize(this->_numPlayers);
	this->_initModules();
	this->_startNewRound();
}

void						Nibbler::_validateArgs(int boardWidth, int boardHeight, int numPlayers)
{
	// numPlayers must be 1 or 2
	if (!(1 <= numPlayers && numPlayers <= MAX_PLAYERS))
		throw NibblerException("Invalid number of players (currently support 1 xor " + std::to_string(MAX_PLAYERS) + " players)");

	// 1P Game: board size must be at least 7x7
	if (numPlayers == 1)
	{
		if (!(7 <= boardWidth && boardWidth <= MAX_BOARD_LEN &&
			7 <= boardHeight && boardHeight <= MAX_BOARD_LEN))
		throw NibblerException("Invalid board size (1P board min size: 7x7, max size: " + std::to_string(MAX_BOARD_LEN) + "x" + std::to_string(MAX_BOARD_LEN) + ")");
	}
	// 2P Game: board size must be at least 14x14
	else
	{
		if (!(14 <= boardWidth && boardWidth <= MAX_BOARD_LEN &&
			14 <= boardHeight && boardHeight <= MAX_BOARD_LEN))
		throw NibblerException("Invalid board size (2P board min size: 14x14, max size: " + std::to_string(MAX_BOARD_LEN) + "x" + std::to_string(MAX_BOARD_LEN) + ")");
	}
}

#include <dlfcn.h>

static IModule *			createModule(std::string libName, std::string funcName, int boardWidth, int boardHeight, std::string title)
{
	void *					dl_handle;
	void *					func_ptr;
	IModule *				module;

	dl_handle = dlopen(libName.c_str(), RTLD_LAZY | RTLD_LOCAL);
	if (dl_handle == nullptr)
		throw NibblerException("dlopen() failed on " + libName);

	func_ptr = dlsym(dl_handle, funcName.c_str());
	if (func_ptr == nullptr)
		throw NibblerException("dlsym() failed on " + libName + " : " + funcName);

	module = reinterpret_cast<IModule * (*)(int, int, std::string)>(func_ptr)(boardWidth, boardHeight, title);
	if (module == nullptr)
		throw NibblerException("createModule() failed on " + libName + " : " + funcName);

	dlclose(dl_handle);
	return (module);
}

void						Nibbler::_initModules(void)
{
	std::string				titleModifier;

	titleModifier = this->_server != nullptr ? "Server" : this->_client != nullptr ? "Client" : "Local";
	titleModifier = " (" + titleModifier + ")";

	this->_modules[0] = createModule("myLibSFML.so", "createSFMLModule", this->_board->getWidth(), this->_board->getHeight(), "SFML" + titleModifier);
	this->_modules[0]->disable();

	this->_modules[1] = createModule("myLibSDL.so", "createSDLModule", this->_board->getWidth(), this->_board->getHeight(), "SDL" + titleModifier);
	this->_modules[1]->disable();

	this->_modules[2] = createModule("myLibOpenGL.so", "createOpenGLModule", this->_board->getWidth(), this->_board->getHeight(), "OpenGL" + titleModifier);
	this->_modules[2]->disable();

	this->_moduleIndex = 0;
	this->_modules[this->_moduleIndex]->enable();
}

// need at least 7x7 board
Snake *						Nibbler::_initSnakeFor1PGame(int playerIndex)
{
	int						xCenter = this->_board->getWidth() / 2;
	int						yCenter = this->_board->getHeight() / 2;
	Player &				player = this->_players[playerIndex];

	return (new Snake(player, *this->_board, xCenter, yCenter, rand_direction()));
}

// need at least 14 x 14 board
Snake *						Nibbler::_initSnakeFor2PGame(int playerIndex)
{
	int						xCenter;
	int						yCenter;
	Player &				player = this->_players[playerIndex];

	// P1 snake starts at upper left corner
	if (playerIndex == 0)
	{
		xCenter = this->_board->getWidth() / 4;
		yCenter = this->_board->getHeight() / 4;
	}
	// P2 snake starts at bottom right corner
	else
	{
		xCenter = this->_board->getWidth() / 4 * 3;
		yCenter = this->_board->getHeight() / 4 * 3;
	}
	return (new Snake(player, *this->_board, xCenter, yCenter, rand_direction()));
}

Nibbler::~Nibbler(void)
{
	delete this->_server;
	delete this->_client;

	delete this->_board;

	for (int i = 0; i < N_MODULES; i++)
		delete this->_modules[i];
}

# define MS_PER_UPDATE		200

void						Nibbler::_loop(void)
{
	double 					t = 0.0;
	std::chrono::time_point<std::chrono::high_resolution_clock> previousTime = std::chrono::high_resolution_clock::now();

	while (!this->_exitProgram)
	{
		if (this->_client != nullptr)
			this->_client->receiveMessages();
		if (this->_server != nullptr)
			this->_server->receiveMessages();

		this->_handleEvents();

		// if local or server game, update() after _ ms
		if (this->_client == nullptr)
		{
			std::chrono::time_point<std::chrono::high_resolution_clock> currentTime = std::chrono::high_resolution_clock::now();
			std::chrono::duration<double> elapsedTime = currentTime - previousTime;
			std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(elapsedTime);
			t += ms.count();
			previousTime = currentTime;

			if (t >= MS_PER_UPDATE)
			{
				if (this->_server != nullptr)
					this->_server->sendMessage(UPDATE_NOW);

				this->update();
				t -= MS_PER_UPDATE;
			}			
		}
		this->_render();
	}
}

void						Nibbler::_handleEvents(void)
{
	IModule *				module = this->_modules[this->_moduleIndex];
	std::vector<e_event>	events = module->getEvents();

	for (e_event & e : events)
		this->_processEvent(e);
}

void		Nibbler::_processEvent(e_event & event)
{
	switch (event)
	{
		case EVENT_TERMINATE:
			this->terminate();
			break;
		case EVENT_P1_TURN_LEFT:
			this->turnLeftP1();
			break;
		case EVENT_P1_TURN_RIGHT:
			this->turnRightP1();
			break;
		case EVENT_P2_TURN_LEFT:
			this->turnLeftP2();
			break;
		case EVENT_P2_TURN_RIGHT:
			this->turnRightP2();
			break;
		case EVENT_START_NEW_ROUND:
			this->startNewRound();
			break;
		case EVENT_SELECT_MODULE_1:
			this->selectModule1();
			break;
		case EVENT_SELECT_MODULE_2:
			this->selectModule2();
			break;
		case EVENT_SELECT_MODULE_3:
			this->selectModule3();
			break;
		default:
			break;
	}
}

void		Nibbler::update(void)
{
	this->_displayGameStatus();

	if (this->_isRoundOver)
		return;

	for (std::unique_ptr<Snake> & snake : this->_snakes)
		snake->update();

	this->_checkIfRoundIsOver();
	if (this->_isRoundOver)
		return;

	for (std::unique_ptr<Enemy> & enemy : this->_enemies)
		enemy->update();

	this->_checkIfRoundIsOver();
	if (this->_isRoundOver)
		return;

	// remove all dead enemies
	this->_enemies.erase(
		std::remove_if(this->_enemies.begin(), this->_enemies.end(), [](std::unique_ptr<Enemy> & enemy){ return (enemy->isDead()); }),
		this->_enemies.end());

	if (this->_client == nullptr)
		this->_spawnEnemies();

	this->_checkIfRoundIsOver();
}

void		Nibbler::spawnEnemy(int enemyID, int posX, int posY, e_direction direction)
{
	// if local or server game, spawn a random enemy
	this->_enemies.push_back(std::unique_ptr<Enemy>(new Enemy(enemyID, posX, posY, direction, *this->_board)));
}

void		Nibbler::_spawnEnemy(int posX, int posY, e_direction direction)
{
	// as client, explicitly spawn an enemy from message
	this->_enemies.push_back(std::unique_ptr<Enemy>(new Enemy(posX, posY, direction, *this->_board)));

	if (this->_server != nullptr)
		this->_server->sendEnemySpawnInfo(*this->_enemies.back());
}

# define SPAWN_RATE			5

void						Nibbler::_spawnEnemies(void)
{
	// spawn enemies that go WEST
	if (std::rand() % 100 < SPAWN_RATE)
	{
		int		x = this->_board->getWidth() - 1;
		int		y = std::rand() % this->_board->getHeight();

		if (this->_board->isEmptyCell(x, y))
			this->_spawnEnemy(x, y, WEST);
	}
	// spawn enemies that go EAST
	if (std::rand() % 100 < SPAWN_RATE)
	{
		int		x = 0;
		int		y = std::rand() % this->_board->getHeight();

		if (this->_board->isEmptyCell(x, y))
			this->_spawnEnemy(x, y, EAST);
	}
	// spawn enemies that go SOUTH
	if (std::rand() % 100 < SPAWN_RATE)
	{
		int		x = std::rand() % this->_board->getWidth();
		int		y = 0;

		if (this->_board->isEmptyCell(x, y))
			this->_spawnEnemy(x, y, SOUTH);
	}
	// spawn enemies that go NORTH
	if (std::rand() % 100 < SPAWN_RATE)
	{
		int		x = std::rand() % this->_board->getWidth();
		int		y = this->_board->getHeight() - 1;

		if (this->_board->isEmptyCell(x, y))
			this->_spawnEnemy(x, y, NORTH);
	}
}

void							Nibbler::_render(void)
{
	IModule	*					module = this->_modules[this->_moduleIndex];
	std::vector<t_cell_data>	cellData = this->_board->getCellData();

	// bool						didIt = false;

	for (t_cell_data & data : cellData)
	{
		if (data.type == CELL_SNAKE)
		{
			data.isActivePlayer = (data.isPlayer0 && (this->_client == nullptr)) ||
				(!data.isPlayer0 && (this->_client != nullptr));

			// didIt = true;
		}
	}

	// assert(didIt == true);

	module->render(cellData);
}

void		Nibbler::_checkIfRoundIsOver(void)
{
	// 1P Game
	if (this->_numPlayers == 1)
	{
		// P1 dieded
		if (this->_snakes[0]->isDead())
		{
			this->_isRoundOver = true;
			this->_winner = "Nobody";
			AudioManager::getInstance().playSFX("1p_lose");
		}
		// P1 won
		else if (this->_snakes[0]->getLength() >= LENGTH_TO_WIN)
		{
			this->_isRoundOver = true;
			this->_winner = "Player 1";
			AudioManager::getInstance().playSFX("1p_win");
			this->_players[0].incrementWinCount();
		}
	}
	// 2P Game
	else
	{
		// All dieded
		if (this->_snakes[0]->isDead() && this->_snakes[1]->isDead())
		{
			this->_isRoundOver = true;
			this->_winner = "Nobody";
			AudioManager::getInstance().playSFX("2p_all_lose");
		}
		// P1 won
		else if (this->_snakes[1]->isDead() || this->_snakes[0]->getLength() >= LENGTH_TO_WIN)
		{
			this->_isRoundOver = true;
			this->_winner = "Player 1";
			AudioManager::getInstance().playSFX("2p_p1_win");
			this->_players[0].incrementWinCount();
		}
		// P2 won
		else if (this->_snakes[0]->isDead()  || this->_snakes[1]->getLength() >= LENGTH_TO_WIN)
		{
			this->_isRoundOver = true;
			this->_winner = "Player 2";
			AudioManager::getInstance().playSFX("2p_p2_win");
			this->_players[1].incrementWinCount();
		}
	}
}

void		Nibbler::_displayGameStatus(void)
{
	system("clear");

	printf("Win Condition: First to length %d wins the round!\n\n", LENGTH_TO_WIN);
	printf("Round %d\n", this->_roundNumber);
	printf("%9s %12s  %12s  %12s  %12s\n", "", "Status", "Length", "Score", "Wins");
	for (int i = 0; i < this->_numPlayers; i++)
	{
		Player & player = this->_players[i];
		std::string status = this->_snakes[i]->isDead() ? "Dead" : "Alive";
		printf("Player %d: %12s  %12d  %12d  %12d\n",
			i + 1,
			status.c_str(),
			this->_snakes[i]->getLength(),
			player.getScore(),
			player.getWinCount());
	}
	if (this->_isRoundOver)
		printf("\nWinner: %s\n", this->_winner.c_str());
}

void		Nibbler::terminate(bool fromOnlineMessage)
{
	this->_exitProgram = true;

	// if not from message, then send a message to the other side
	if (!fromOnlineMessage)
	{
		if (this->_server != nullptr)
			this->_server->sendMessage(TERMINATE);	
		if (this->_client != nullptr)
			this->_client->sendMessage(TERMINATE);
	}
}

void		Nibbler::_startNewRound(void)
{
	this->_isRoundOver = false;
	this->_roundNumber++;
	this->_board->clearAllCells();
	this->_enemies.clear();

	// if local or server game, then spawn snakes, spawn food, and select bgm
	if (this->_client == nullptr)
	{
		for (int i = 0; i < this->_numPlayers; i++)
		{
			if (this->_numPlayers == 1)
				this->_snakes[i] = std::unique_ptr<Snake>(this->_initSnakeFor1PGame(i));
			else
				this->_snakes[i] = std::unique_ptr<Snake>(this->_initSnakeFor2PGame(i));
		}
		FoodCell &	foodCell = this->_board->generateFood();
		int			bgmID = AudioManager::getInstance().playBGM();

		if (this->_server != nullptr)
			this->_server->sendStartNewRoundInfo(*this->_snakes[0], *this->_snakes[1], foodCell, bgmID);
	}	
}

void		Nibbler::spawnFood(void)
{
	// if local or server game, then spawn food
	if (this->_client == nullptr)
	{
		FoodCell & foodCell = this->_board->generateFood();

		if (this->_server != nullptr)
			this->_server->sendFoodSpawnInfo(foodCell);
	}
}

void		Nibbler::spawnFood(int foodID, int posX, int posY)
{
	// as client, explicitly spawn food from message
	this->_board->generateFood(foodID, posX, posY);
}

void		Nibbler::spawnSnake(int playerID, int posX, int posY, e_direction direction)
{
	// as client, explicitly spawn snake from message
	this->_snakes[playerID] = std::unique_ptr<Snake>(new Snake(this->_players[playerID], *this->_board, posX, posY, direction));
}

void		Nibbler::startNewRound(bool fromOnlineMessage)
{
	// if local or server game, or if client received message, then start new round
	if (fromOnlineMessage || (this->_client == nullptr))
		this->_startNewRound();
}

void		Nibbler::turnLeftP1(bool fromOnlineMessage)
{
	// if local or server game, or if client received message, then P1 turn left
	if (fromOnlineMessage || (this->_client == nullptr))
		this->_snakes[0]->turnLeft();

	if (this->_server != nullptr)
		this->_server->sendMessage(P1_TURN_LEFT);
}

void		Nibbler::turnRightP1(bool fromOnlineMessage)
{
	// if local or server game, or if client received message, then P1 turn right
	if (fromOnlineMessage || (this->_client == nullptr))
		this->_snakes[0]->turnRight();

	if (this->_server != nullptr)
		this->_server->sendMessage(P1_TURN_RIGHT);
}

void		Nibbler::turnLeftP2(bool fromOnlineMessage)
{
	// local
	if (this->_numPlayers == 2 && (this->_server == nullptr) && (this->_client == nullptr))
	{
		this->_snakes[1]->turnLeft();
	}
	// server
	else if (this->_server != nullptr)
	{
		if (fromOnlineMessage)
		{
			this->_snakes[1]->turnLeft();
			this->_server->sendMessage(P2_TURN_LEFT);
		}
	}
	// client
	else if (this->_client != nullptr)
	{
		if (fromOnlineMessage)
			this->_snakes[1]->turnLeft();
		else
			this->_client->sendMessage(P2_TURN_LEFT);
	}
}

void		Nibbler::turnRightP2(bool fromOnlineMessage)
{
	// local
	if (this->_numPlayers == 2 && (this->_server == nullptr) && (this->_client == nullptr))
	{
		this->_snakes[1]->turnRight();
	}
	// server
	else if (this->_server != nullptr)
	{
		if (fromOnlineMessage)
		{
			this->_snakes[1]->turnRight();
			this->_server->sendMessage(P2_TURN_RIGHT);
		}
	}
	// client
	if (this->_client != nullptr)
	{
		if (fromOnlineMessage)
			this->_snakes[1]->turnRight();
		else
			this->_client->sendMessage(P2_TURN_RIGHT);
	}
}

void		Nibbler::selectModule1(bool fromOnlineMessage)
{
	// if local or server game, or if client received message, switch to module 1
	if (fromOnlineMessage || (this->_client == nullptr))
		this->_toggleModules(0);

	if (this->_server != nullptr)
		this->_server->sendMessage(SELECT_MODULE_1);
}

void		Nibbler::selectModule2(bool fromOnlineMessage)
{
	// if local or server game, or if client received message, switch to module 2
	if (fromOnlineMessage || (this->_client == nullptr))
		this->_toggleModules(1);

	if (this->_server != nullptr)
		this->_server->sendMessage(SELECT_MODULE_2);
}

void		Nibbler::selectModule3(bool fromOnlineMessage)
{
	// if local or server game, or if client received message, switch to module 3
	if (fromOnlineMessage || (this->_client == nullptr))
		this->_toggleModules(2);

	if (this->_server != nullptr)
		this->_server->sendMessage(SELECT_MODULE_3);
}

void		Nibbler::_toggleModules(int nextIndex)
{
	if (this->_moduleIndex != nextIndex)
	{
		for (int i = 0; i < N_MODULES; i++)
			this->_modules[i]->disable();

		this->_moduleIndex = nextIndex;
		this->_modules[this->_moduleIndex]->enable();
	}
}
