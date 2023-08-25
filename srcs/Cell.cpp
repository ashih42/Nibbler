#include "Cell.hpp"

Cell::Cell(int x, int y) :
	_x(x),
	_y(y)
{

}

Cell::~Cell(void)
{
	
}

int			Cell::getX(void) const
{
	return (this->_x);
}

int			Cell::getY(void) const
{
	return (this->_y);
}

void		Cell::setXY(int x, int y)
{
	this->_x = x;
	this->_y = y;
}
