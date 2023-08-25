#ifndef NIBBLER_HPP
# define NIBBLER_HPP

# include "Player.hpp"
# include "direction.hpp"
# include "event.hpp"

# include <string>
# include <iostream>
# include <vector>

# define MAX_PLAYERS						2
# define N_MODULES							3
# define LENGTH_TO_WIN						10

# define MAX_BOARD_LEN						50

class IModule;
class Board;
class Snake;
class Enemy;
class Server;
class Client;

class Nibbler
{
public:

	static void								createLocalGame(int boardWidth, int boardHeight, int numPlayers);
	static void								createOnlineGameAsServer(int boardWidth, int boardHeight, unsigned short port);
	static void								createOnlineGameAsClient(std::string & ipAddress, unsigned short port);

	static Nibbler &						getInstance(void);

	~Nibbler(void);

	void									terminate(bool fromOnlineMessage=false);

	void									selectModule1(bool fromOnlineMessage=false);
	void									selectModule2(bool fromOnlineMessage=false);
	void									selectModule3(bool fromOnlineMessage=false);

	void									turnLeftP1(bool fromOnlineMessage=false);
	void									turnRightP1(bool fromOnlineMessage=false);
	void									turnLeftP2(bool fromOnlineMessage=false);
	void									turnRightP2(bool fromOnlineMessage=false);

	void									startNewRound(bool fromOnlineMessage=false);

	void									spawnFood(void);
	void									spawnFood(int foodID, int posX, int posY);
	void									spawnSnake(int playerID, int posX, int posY, e_direction direction);
	void									spawnEnemy(int enemyID, int posX, int posY, e_direction direction);

	void									update(void);

private:

	static Nibbler *						_instance;

	int										_numPlayers;

	bool									_exitProgram;
	bool									_isRoundOver;
	int										_roundNumber;
	std::string								_winner;

	Board *									_board;

	IModule *								_modules[N_MODULES];
	int										_moduleIndex;

	Player									_players[MAX_PLAYERS];
	std::vector<std::unique_ptr<Snake>>		_snakes;
	std::vector<std::unique_ptr<Enemy>>		_enemies;

	Server *								_server;
	Client *								_client;

	Nibbler(int boardWidth, int boardHeight, int numPlayers);			// local game constructor
	Nibbler(int boardWidth, int boardHeight, unsigned short port);		// online server constructor
	Nibbler(std::string & ipAddress, unsigned short port);				// online client constructor

	Nibbler(void);
	Nibbler(const Nibbler & src);
	Nibbler & operator=(const Nibbler & rhs);

	void									_validateArgs(int boardWidth, int boardHeight, int numPlayers);

	void									_initModules(void);
	void									_toggleModules(int nextIndex);

	Snake *									_initSnakeFor1PGame(int playerIndex);
	Snake *									_initSnakeFor2PGame(int playerIndex);
	
	void									_startNewRound(void);

	void									_loop(void);
	void									_handleEvents(void);
	void									_processEvent(e_event & event);
	void									_render(void);
	void									_displayGameStatus(void);

	void									_spawnEnemies(void);
	void									_spawnEnemy(int posX, int posY, e_direction direction);

	void									_checkIfRoundIsOver(void);

};

#endif
