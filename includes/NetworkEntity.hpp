#ifndef NETWORK_ENTITY_HPP
# define NETWORK_ENTITY_HPP

# include "message.hpp"

# include <iostream>
# include <SFML/Network.hpp>

class NetworkEntity
{
public:

	virtual ~NetworkEntity(void);

	void					sendMessage(e_message message);
	virtual void			receiveMessages(void) = 0;

protected:

	sf::TcpSocket			_socket;

	NetworkEntity(void);

private:

	NetworkEntity(const NetworkEntity & src);
	NetworkEntity &	operator=(const NetworkEntity & rhs);

};

#endif
