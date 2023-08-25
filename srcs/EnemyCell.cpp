#include "EnemyCell.hpp"
#include "Enemy.hpp"
#include "Board.hpp"

EnemyCell::EnemyCell(int x, int y, Enemy & enemy) :
	Cell(x, y),
	_enemy(enemy)
{
	
}

EnemyCell::~EnemyCell(void)
{
	
}

int				EnemyCell::getID(void) const
{
	return (this->_enemy.getID());
}

e_direction		EnemyCell::getDirection(void) const
{
	return (this->_enemy.getDirection());
}

t_cell_data		EnemyCell::getCellData(void) const
{
	t_cell_data	data;

	data.type = CELL_ENEMY;
	data.posX = this->_x;
	data.posY = this->_y;
	data.direction = this->getDirection();
	data.id = this->getID();
	return (data);
}

void			EnemyCell::getHit(void)
{
	// Enemy stays on the board
}
