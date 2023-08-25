#ifndef ENEMY_CELL_HPP
# define ENEMY_CELL_HPP

# include "Cell.hpp"
# include "direction.hpp"

class Board;
class Enemy;

class EnemyCell : public Cell
{
public:

	EnemyCell(int x, int y, Enemy & enemy);
	~EnemyCell(void);

	int						getID(void) const;
	e_direction				getDirection(void) const;
	virtual t_cell_data		getCellData(void) const;

	virtual void			getHit(void);

private:

	Enemy &					_enemy;
	int						_id;

	EnemyCell(void);
	EnemyCell(const EnemyCell & src);
	EnemyCell & operator=(const EnemyCell & rhs);

};

#endif
