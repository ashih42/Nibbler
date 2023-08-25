#include "SnakeCell.hpp"
#include "Snake.hpp"

SnakeCell::SnakeCell(int x, int y, Snake & snake, e_direction direction, bool isHead, size_t index) :
	Cell(x, y),
	_snake(snake),
	_direction(direction),
	_isHead(isHead),
	_index(index)
{
	
}

SnakeCell::~SnakeCell(void)
{

}

Snake &				SnakeCell::getSnake(void) const
{
	return (this->_snake);
}

e_direction			SnakeCell::getDirection(void) const
{
	return (this->_direction);
}

bool				SnakeCell::isHead(void) const
{
	return (this->_isHead);
}

t_cell_data			SnakeCell::getCellData(void) const
{
	t_cell_data		data;

	data.type = CELL_SNAKE;
	data.posX = this->_x;
	data.posY = this->_y;
	data.isPlayer0 = this->getSnake().getPlayerID() == 0;
	data.isHead = this->_isHead;
	data.isDead = this->getSnake().isDead();
	data.direction = this->getDirection();
	return (data);
}

void				SnakeCell::setDirection(e_direction direction)
{
	this->_direction = direction;
}

void				SnakeCell::getHit(void)
{
	if (this->_isHead)
		this->_snake.die();
	else
		this->_snake.getHitAtIndex(this->_index);
}
