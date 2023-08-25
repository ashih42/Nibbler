#include "Player.hpp"

int		Player::_numPlayers = 0;

Player::Player(void) : _winCount(0), _score(0)
{
	this->_id = Player::_numPlayers++;
}

Player::~Player(void) { }

int		Player::getID(void) const
{
	return (this->_id);
}

int		Player::getWinCount(void) const
{
	return (this->_winCount);
}

int		Player::getScore(void) const
{
	return (this->_score);
}

void	Player::incrementWinCount(void)
{
	this->_winCount++;
}

void	Player::incrementScore(void)
{
	this->_score++;
}
