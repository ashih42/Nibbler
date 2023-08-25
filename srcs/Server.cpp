#include "Server.hpp"
#include "Nibbler.hpp"
#include "NibblerException.hpp"
#include "message.hpp"
#include "Board.hpp"
#include "Snake.hpp"
#include "SnakeCell.hpp"
#include "FoodCell.hpp"
#include "Enemy.hpp"
#include "EnemyCell.hpp"

Server::Server(unsigned short port) :
	NetworkEntity()
{
	if (this->_listener.listen(port) != sf::Socket::Done)
		throw NibblerException("sf::TcpListener::listen() failed");

	std::cout << "[Server] Waiting for connection..." << std::endl;

	if (this->_listener.accept(this->_socket) != sf::Socket::Done)
		throw NibblerException("sf::TcpSocket::accept() failed");	

	std::cout << "[Server] Connection successful!" << std::endl;

	this->_listener.close();
	this->_socket.setBlocking(false);
}

Server::~Server(void)
{
	this->_socket.disconnect();
}

void					Server::sendBoardInfo(Board & board)
{
	sf::Packet			packet;
	
	packet << sf::Int8(BOARD_INFO)
		<< sf::Int32(board.getWidth())
		<< sf::Int32(board.getHeight());
	this->_socket.send(packet);
}

void					Server::sendStartNewRoundInfo(Snake & snake0, Snake & snake1, FoodCell & foodCell, int bgmID)
{
	sf::Packet			packet;
	SnakeCell &			snakeHeadCell0 = snake0.getHeadCell();
	SnakeCell &			snakeHeadCell1 = snake1.getHeadCell();

	packet << sf::Int8(START_NEW_ROUND)
		// Player 1 snake info
		<< sf::Int8(0)
		<< sf::Int32(snakeHeadCell0.getX())
		<< sf::Int32(snakeHeadCell0.getY())
		<< sf::Int8(snakeHeadCell0.getDirection())
		// Player 2 snake info
		<< sf::Int8(1)
		<< sf::Int32(snakeHeadCell1.getX())
		<< sf::Int32(snakeHeadCell1.getY())
		<< sf::Int8(snakeHeadCell1.getDirection())
		// Food info
		<< sf::Int32(foodCell.getID())
		<< sf::Int32(foodCell.getX())
		<< sf::Int32(foodCell.getY())
		// BGM info
		<< sf::Int8(bgmID);
	this->_socket.send(packet);
}

void					Server::sendFoodSpawnInfo(FoodCell & foodCell)
{
	sf::Packet			packet;

	packet << sf::Int8(FOOD_SPAWNED)
		<< sf::Int32(foodCell.getID())
		<< sf::Int32(foodCell.getX())
		<< sf::Int32(foodCell.getY());
	this->_socket.send(packet);
}

void					Server::sendEnemySpawnInfo(Enemy & enemy)
{
	sf::Packet			packet;
	EnemyCell &			enemyCell = enemy.getEnemyCell();

	packet << sf::Int8(ENEMY_SPAWNED)
		<< sf::Int32(enemyCell.getID())
		<< sf::Int32(enemyCell.getX())
		<< sf::Int32(enemyCell.getY())
		<< sf::Int8(enemyCell.getDirection());
	this->_socket.send(packet);
}

void					Server::receiveMessages(void)
{
	sf::Packet			packet;
	sf::Socket::Status	status;

	sf::Int8			message_;
	e_message			message;

	while ((status = this->_socket.receive(packet)) == sf::Socket::Done)
	{
		if (!(packet >> message_))
		{
			std::cout << "Server::receiveMessages(): bad packet" << std::endl;
			return;
		}
		message = static_cast<e_message>(message_);
		switch (message)
		{
			case P2_TURN_LEFT:
				Nibbler::getInstance().turnLeftP2(true);
				break;
			case P2_TURN_RIGHT:
				Nibbler::getInstance().turnRightP2(true);
				break;
			case TERMINATE:
				Nibbler::getInstance().terminate(true);
				break;
			default:
				std::cout << "Server::receiveMessages(): dafuq is this 🤷" << std::endl;
				break;
		}
	}
}
