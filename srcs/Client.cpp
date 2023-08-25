#include "Client.hpp"
#include "Nibbler.hpp"
#include "NibblerException.hpp"
#include "message.hpp"
#include "direction.hpp"
#include "AudioManager.hpp"

Client::Client(std::string & ipAddress, unsigned short port) :
	NetworkEntity()
{
	std::cout << "[Client] Attempting to connect to server at " << ipAddress << ":" << port << " ..." << std::endl;

	if (this->_socket.connect(ipAddress, port) != sf::Socket::Done)
		throw NibblerException("sf::TcpSocket::connect() failed");

	std::cout << "[Client] Connection successful!" << std::endl;

	this->_socket.setBlocking(false);
}

Client::~Client(void)
{
	this->_socket.disconnect();
}

void					Client::receiveBoardInfo(int & boardWidth, int & boardHeight)
{
	sf::Packet			packet;
	sf::Socket::Status	status;

	sf::Int8			message_;
	sf::Int32			width_;
	sf::Int32			height_;

	e_message			message;

	while ((status = this->_socket.receive(packet)) != sf::Socket::Done)
		;

	if (!(packet >> message_ >> width_ >> height_))
	{
		std::cout << "Client::receiveBoardInfo(): bad packet" << std::endl;
		return;
	}
	message = static_cast<e_message>(message_);
	if (message != BOARD_INFO)
	{
		std::cout << "Client::receiveBoardInfo(): wrong message" << std::endl;
		return;
	}
	boardWidth = static_cast<int>(width_);
	boardHeight = static_cast<int>(height_);
}

void					Client::receiveMessages(void)
{
	sf::Packet			packet;
	sf::Socket::Status	status;

	sf::Int8			message_;
	e_message			message;

	while ((status = this->_socket.receive(packet)) == sf::Socket::Done)
	{
		if (!(packet >> message_))
		{
			std::cout << "Client::receiveMessages(): bad packet \n" << std::endl;
			return;
		}
		message = static_cast<e_message>(message_);
		switch (message)
		{
			case P1_TURN_LEFT:
				Nibbler::getInstance().turnLeftP1(true);
				break;
			case P1_TURN_RIGHT:
				Nibbler::getInstance().turnRightP1(true);
				break;
			case P2_TURN_LEFT:
				Nibbler::getInstance().turnLeftP2(true);
				break;
			case P2_TURN_RIGHT:
				Nibbler::getInstance().turnRightP2(true);
				break;
			case SELECT_MODULE_1:
				Nibbler::getInstance().selectModule1(true);
				break;
			case SELECT_MODULE_2:
				Nibbler::getInstance().selectModule2(true);
				break;
			case SELECT_MODULE_3:
				Nibbler::getInstance().selectModule3(true);
				break;
			case FOOD_SPAWNED:
				this->_handleFoodSpawnInfo(packet);
				break;
			case ENEMY_SPAWNED:
				this->_handleEnemySpawnInfo(packet);
				break;
			case START_NEW_ROUND:
				this->_handleStartNewRoundInfo(packet);
				break;
			case UPDATE_NOW:
				Nibbler::getInstance().update();
				break;
			case TERMINATE:
				Nibbler::getInstance().terminate(true);
				break;
			default:
				std::cout << "Client::receiveMessages(): dafuq is this 🤷 \n" << std::endl;
				break;
		}
	}
}

void					Client::_handleStartNewRoundInfo(sf::Packet & packet)
{
	Nibbler::getInstance().startNewRound(true);
	this->_handleSnakeSpawnInfo(packet);
	this->_handleSnakeSpawnInfo(packet);
	this->_handleFoodSpawnInfo(packet);
	this->_handleBGMInfo(packet);
}

void					Client::_handleBGMInfo(sf::Packet & packet)
{
	sf::Int8			bgmID_;

	int					bgmID;

	if (!(packet >> bgmID_))
	{
		std::cout << "Client::_handleBGMInfo(): bad packet \n" << std::endl;
		return;
	}
	bgmID = static_cast<int>(bgmID_);
	AudioManager::getInstance().playBGM(bgmID);
}

void					Client::_handleSnakeSpawnInfo(sf::Packet & packet)
{
	sf::Int8			playerID_;
	sf::Int32			posX_;
	sf::Int32			posY_;
	sf::Int8			direction_;

	int					playerID;
	int					posX;
	int					posY;
	e_direction			direction;

	if (!(packet >> playerID_ >> posX_ >> posY_ >> direction_))
	{
		std::cout << "Client::_handleSnakeSpawnInfo(): bad packet \n" << std::endl;
		return;
	}
	playerID = static_cast<int>(playerID_);
	posX = static_cast<int>(posX_);
	posY = static_cast<int>(posY_);
	direction = static_cast<e_direction>(direction_);

	Nibbler::getInstance().spawnSnake(playerID, posX, posY, direction);
}

void					Client::_handleFoodSpawnInfo(sf::Packet & packet)
{
	sf::Int32			foodID_;
	sf::Int32			posX_;
	sf::Int32			posY_;

	int					foodID;
	int					posX;
	int					posY;

	if (!(packet >> foodID_ >> posX_ >> posY_))
	{
		std::cout << "Client::_handleFoodSpawnInfo(): bad packet \n" << std::endl;
		return;
	}
	foodID = static_cast<int>(foodID_);
	posX = static_cast<int>(posX_);
	posY = static_cast<int>(posY_);
	
	Nibbler::getInstance().spawnFood(foodID, posX, posY);
}

void					Client::_handleEnemySpawnInfo(sf::Packet & packet)
{
	sf::Int32			enemyID_;
	sf::Int32			posX_;
	sf::Int32			posY_;
	sf::Int8			direction_;

	int					enemyID;
	int					posX;
	int					posY;
	e_direction			direction;

	if (!(packet >> enemyID_ >> posX_ >> posY_ >> direction_))
	{
		std::cout << "Client::_handleFoodSpawnInfo(): bad packet \n" << std::endl;
		return;
	}
	enemyID = static_cast<int>(enemyID_);
	posX = static_cast<int>(posX_);
	posY = static_cast<int>(posY_);
	direction = static_cast<e_direction>(direction_);

	Nibbler::getInstance().spawnEnemy(enemyID, posX, posY, direction);
}
