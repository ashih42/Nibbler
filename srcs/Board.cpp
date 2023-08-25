#include "Board.hpp"
#include "SnakeCell.hpp"
#include "FoodCell.hpp"
#include "NibblerException.hpp"

#include <iostream>

Board::Board(int width, int height) :
	_width(width),
	_height(height)
{
	this->_cells.resize(this->_width * this->_height);
}

Board::~Board(void)
{
	this->_cells.clear();
}

int			Board::getWidth(void) const
{
	return (this->_width);
}

int			Board::getHeight(void) const
{
	return (this->_height);
}

Cell *		Board::getCell(int x, int y) const
{
	return (this->_cells[this->_width * y + x].get());
}

bool		Board::isEmptyCell(int x, int y) const
{
	return (this->_cells[this->_width * y + x].get() == nullptr);
}

bool		Board::isValidPosition(int x, int y) const
{
	return (0 <= x && x < this->_width && 0 <= y && y < this->_height);
}

std::vector<t_cell_data>		Board::getCellData(void) const
{
	std::vector<t_cell_data>	dataData;

	for (std::shared_ptr<Cell> const & cell : this->_cells)
		if (cell.get() != nullptr)
			dataData.push_back(cell->getCellData());

	return (dataData);
}

void		Board::setCell(const std::shared_ptr<Cell> & cell)
{
	int		x = cell->getX();
	int		y = cell->getY();

	this->_cells[this->_width * y + x] = cell;
}

void		Board::setCell(const std::shared_ptr<Cell> & cell, int x, int y)
{
	cell->setXY(x, y);
	this->_cells[this->_width * y + x] = cell;
}

void		Board::clearCell(int x, int y)
{
	this->_cells[this->_width * y + x].reset();
}

void		Board::clearAllCells(void)
{
	for (std::shared_ptr<Cell> & cell : this->_cells)
		cell.reset();
}

void		Board::_findEmptyPosition(int & emptyX, int & emptyY)
{
	int		startX = std::rand() % this->_width;
	int		startY = std::rand() % this->_height;

	int		x = startX;
	int		y = startY;

	while (true)
	{
		if (this->getCell(x, y) == nullptr)
		{
			emptyX = x;
			emptyY = y;
			return;
		}
		x += 1;
		if (x == this->_width)
		{
			x = 0;
			y = (y + 1) % this->_height;
		}
		if (x == startX && y == startY)
			throw NibblerException("Board::_findEmptyPosition() failed");
	}
}

FoodCell &	Board::generateFood(void)
{
	int		emptyX;
	int		emptyY;

	this->_findEmptyPosition(emptyX, emptyY);
	this->setCell(std::make_shared<FoodCell>(emptyX, emptyY, *this));
	return (static_cast<FoodCell &>(*this->getCell(emptyX, emptyY)));
}

void		Board::generateFood(int id, int posX, int posY)
{
	this->setCell(std::make_shared<FoodCell>(id, posX, posY, *this));
}
