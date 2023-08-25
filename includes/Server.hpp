#ifndef SERVER_HPP
# define SERVER_HPP

# include "NetworkEntity.hpp"

class Board;
class Snake;
class FoodCell;
class Enemy;

class Server : public NetworkEntity
{
public:
	
	Server(unsigned short port);
	~Server(void);

	void				sendBoardInfo(Board & board);
	void				sendStartNewRoundInfo(Snake & snake0, Snake & snake1, FoodCell & foodCell, int bgmID);
	void				sendFoodSpawnInfo(FoodCell & foodCell);
	void				sendEnemySpawnInfo(Enemy & enemy);

	virtual void		receiveMessages(void);

private:

	sf::TcpListener		_listener;

	Server(void);
	Server(const Server & src);
	Server & operator=(const Server & rhs);

};

#endif
