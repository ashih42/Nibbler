#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "NetworkEntity.hpp"

class Client : public NetworkEntity
{
public:

	Client(std::string & ipAddress, unsigned short port);
	~Client(void);

	void				receiveBoardInfo(int & boardWidth, int & boardHeight);

	virtual void		receiveMessages(void);

private:

	Client(void);
	Client(const Client & src);
	Client & operator=(const Client & rhs);

	void				_handleStartNewRoundInfo(sf::Packet & packet);

	void				_handleBGMInfo(sf::Packet & packet);
	void				_handleSnakeSpawnInfo(sf::Packet & packet);
	void				_handleFoodSpawnInfo(sf::Packet & packet);
	void				_handleEnemySpawnInfo(sf::Packet & packet);

};

#endif
