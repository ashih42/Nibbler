#include "NetworkEntity.hpp"
#include "Nibbler.hpp"

NetworkEntity::NetworkEntity(void) { }

NetworkEntity::~NetworkEntity(void) { }

void			NetworkEntity::sendMessage(e_message message)
{
	sf::Packet	packet;
	
	packet << sf::Int8(message);
	this->_socket.send(packet);
}
