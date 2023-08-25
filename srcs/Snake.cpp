#include "Snake.hpp"
#include "Player.hpp"
#include "Board.hpp"
#include "Cell.hpp"
#include "SnakeCell.hpp"
#include "FoodCell.hpp"
#include "NibblerException.hpp"
#include "AudioManager.hpp"
#include <iostream>

Snake::Snake(Player & player, Board & board, int x, int y, e_direction direction) :
	_player(player),
	_board(board),
	_isDead(false),
	_nextDirection(direction)
{
	this->_initSnakeCells(x, y);
	this->_registerSnakeCellsToBoard();
}

// build a snake 4 units long
void				Snake::_initSnakeCells(int x, int y)
{
	this->_snakeCells.push_back(std::make_shared<SnakeCell>(x, y, *this, this->_nextDirection, true, 0));
	switch (this->_nextDirection)
	{
		case EAST:
			this->_snakeCells.push_back(std::make_shared<SnakeCell>(x - 1, y, *this, this->_nextDirection, false, 1));
			this->_snakeCells.push_back(std::make_shared<SnakeCell>(x - 2, y, *this, this->_nextDirection, false, 2));
			this->_snakeCells.push_back(std::make_shared<SnakeCell>(x - 3, y, *this, this->_nextDirection, false, 3));
			break;
		case SOUTH:
			this->_snakeCells.push_back(std::make_shared<SnakeCell>(x, y - 1, *this, this->_nextDirection, false, 1));
			this->_snakeCells.push_back(std::make_shared<SnakeCell>(x, y - 2, *this, this->_nextDirection, false, 2));
			this->_snakeCells.push_back(std::make_shared<SnakeCell>(x, y - 3, *this, this->_nextDirection, false, 3));
			break;
		case WEST:
			this->_snakeCells.push_back(std::make_shared<SnakeCell>(x + 1, y, *this, this->_nextDirection, false, 1));
			this->_snakeCells.push_back(std::make_shared<SnakeCell>(x + 2, y, *this, this->_nextDirection, false, 2));
			this->_snakeCells.push_back(std::make_shared<SnakeCell>(x + 3, y, *this, this->_nextDirection, false, 3));
			break;
		case NORTH:
			this->_snakeCells.push_back(std::make_shared<SnakeCell>(x, y + 1, *this, this->_nextDirection, false, 1));
			this->_snakeCells.push_back(std::make_shared<SnakeCell>(x, y + 2, *this, this->_nextDirection, false, 2));
			this->_snakeCells.push_back(std::make_shared<SnakeCell>(x, y + 3, *this, this->_nextDirection, false, 3));
			break;
	}
}

void				Snake::_registerSnakeCellsToBoard(void)
{
	for (const std::shared_ptr<SnakeCell> snakeCell : this->_snakeCells)
	{
		if (!this->_board.isValidPosition(snakeCell->getX(), snakeCell->getY()))
			throw NibblerException("Snake_registerSnakeCells() failed");
		this->_board.setCell(snakeCell);	
	}
}

Snake::~Snake(void)
{

}

bool				Snake::isDead(void) const
{
	return (this->_isDead);
}

int					Snake::getID(void) const
{
	return (this->_player.getID());
}

int					Snake::getLength(void) const
{
	return (static_cast<int>(this->_snakeCells.size()));
}

SnakeCell &			Snake::getHeadCell(void) const
{
	return (*this->_snakeCells[0]);
}

int					Snake::getPlayerID(void) const
{
	return (this->_player.getID());
}

void							Snake::_getNextXY(int & nextX, int & nextY) const
{
	std::shared_ptr<SnakeCell>	headCell = this->_snakeCells.front();
	int							headX = headCell->getX();
	int							headY = headCell->getY();

	switch(this->_nextDirection)
	{
		case EAST:
			nextX = headX + 1;
			nextY = headY;
			break;
		case SOUTH:
			nextX = headX;
			nextY = headY + 1;
			break;
		case WEST:
			nextX = headX - 1;
			nextY = headY;
			break;
		case NORTH:
			nextX = headX;
			nextY = headY - 1;
			break;
	}
}

void				Snake::update(void)
{
	if (this->_isDead)
		return;

	int				nextX;
	int				nextY;
	Cell *			nextCell;

	this->_getNextXY(nextX, nextY);
	if (this->_board.isValidPosition(nextX, nextY))
	{
		if ((nextCell = this->_board.getCell(nextX, nextY)))
			this->_interactWithTarget(*nextCell);
		else
			this->_moveAndGrow(nextX, nextY, false);
	}
	else
		this->die();
}

void				Snake::_interactWithTarget(Cell & target)
{
	int				targetX = target.getX();
	int				targetY = target.getY();

	// consume food, move there, grow 1 cell
	if (dynamic_cast<FoodCell *>(&target))
	{
		target.getHit();
		AudioManager::getInstance().playSFX("eating");
		this->_moveAndGrow(targetX, targetY, true);
		this->_player.incrementScore();
	}
	// collided and dieded
	else
	{
		SnakeCell * enemySnakeCell;
		if ((enemySnakeCell = dynamic_cast<SnakeCell *>(&target)) &&
			enemySnakeCell->isHead())
			enemySnakeCell->getSnake().die();

		this->die();
	}
}


void				Snake::_moveAndGrow(int newX, int newY, bool isGrowing)
{
	int				oldX;
	int				oldY;
	e_direction		oldDirection;
	e_direction		newDirection = this->_nextDirection;

	for (const std::shared_ptr<SnakeCell> snakeCell : this->_snakeCells)
	{
		oldX = snakeCell->getX();
		oldY = snakeCell->getY();
		oldDirection = snakeCell->getDirection();
		this->_board.clearCell(oldX, oldY);
		this->_board.setCell(snakeCell, newX, newY);
		snakeCell->setDirection(newDirection);
		newX = oldX;
		newY = oldY;
		newDirection = oldDirection;
	}
	if (isGrowing)
	{
		int index = static_cast<int>(this->_snakeCells.size());
		std::shared_ptr<SnakeCell> newCell = std::make_shared<SnakeCell>(newX, newY, *this, newDirection, false, index);
		this->_snakeCells.push_back(newCell);
		this->_board.setCell(newCell);
	}
}

void				Snake::turnLeft(void)
{
	e_direction		currentDirection = this->_snakeCells.front()->getDirection();

	switch (currentDirection)
	{
		case EAST:
			this->_nextDirection = NORTH;
			break;
		case SOUTH:
			this->_nextDirection = EAST;
			break;
		case WEST:
			this->_nextDirection = SOUTH;
			break;
		case NORTH:
			this->_nextDirection = WEST;
			break;
	}
}

void				Snake::turnRight(void)
{
	e_direction		currentDirection = this->_snakeCells.front()->getDirection();

	switch (currentDirection)
	{
		case EAST:
			this->_nextDirection = SOUTH;
			break;
		case SOUTH:
			this->_nextDirection = WEST;
			break;
		case WEST:
			this->_nextDirection = NORTH;
			break;
		case NORTH:
			this->_nextDirection = EAST;
			break;
	}
}

void				Snake::getHitAtIndex(size_t index)
{
	AudioManager::getInstance().playSFX("hurt");
	for (size_t i = index; i < this->_snakeCells.size(); i++)
	{
		int x = this->_snakeCells[i]->getX();
		int y = this->_snakeCells[i]->getY();
		this->_board.clearCell(x, y);
	}
	this->_snakeCells.resize(index);
}

void				Snake::die(void)
{
	if (this->_isDead)
		return;

	AudioManager::getInstance().playSFX("death");
	this->_isDead = true;
	for(size_t i = 1; i < this->_snakeCells.size(); i++)
		this->_board.clearCell(this->_snakeCells[i]->getX(), this->_snakeCells[i]->getY());
	this->_snakeCells.resize(1);
}
