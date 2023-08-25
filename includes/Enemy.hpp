#ifndef ENEMY_HPP
# define ENEMY_HPP

# include "direction.hpp"
# include <memory>

class EnemyCell;
class Board;

class Enemy
{
public:

	Enemy(int id, int x, int y, e_direction direction, Board & board);
	Enemy(int x, int y, e_direction direction, Board & board);
	~Enemy(void);

	int							getID(void) const;
	bool						isDead(void) const;
	e_direction					getDirection(void) const;
	EnemyCell &					getEnemyCell(void) const;

	void						update(void);

private:

	int							_id;
	bool						_isDead;
	e_direction					_direction;
	Board &						_board;
	std::shared_ptr<EnemyCell>	_enemyCell;

	Enemy(void);
	Enemy(const Enemy & src);
	Enemy & operator=(const Enemy & rhs);

	void						_getNextXY(int & nextX, int & nextY) const;

};

#endif
