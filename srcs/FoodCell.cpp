#include "FoodCell.hpp"
#include "Board.hpp"
#include "Nibbler.hpp"

FoodCell::FoodCell(int id, int x, int y, Board & board) : Cell(x, y), _board(board)
{
	this->_id = id;
}

FoodCell::FoodCell(int x, int y, Board & board) : Cell(x, y), _board(board)
{
	this->_id = std::rand();
}

FoodCell::~FoodCell(void)
{
	
}

int					FoodCell::getID(void) const
{
	return (this->_id);
}

t_cell_data			FoodCell::getCellData(void) const
{
	t_cell_data		data;

	data.type = CELL_FOOD;
	data.posX = this->_x;
	data.posY = this->_y;
	data.id = this->_id;
	return (data);
}

void				FoodCell::getHit(void)
{
	Nibbler::getInstance().spawnFood();
	this->_board.clearCell(this->_x, this->_y);
}
