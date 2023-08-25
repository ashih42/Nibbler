#ifndef CELL_DATA_HPP
# define CELL_DATA_HPP

# include "direction.hpp"

enum				e_cell_type
{
	CELL_NONE,		// unused
	CELL_SNAKE,
	CELL_FOOD,
	CELL_ENEMY
};

typedef struct		s_cell_data
{
	e_cell_type		type;
	int				posX;
	int				posY;
	bool			isPlayer0;
	bool			isHead;
	bool			isDead;
	bool			isActivePlayer;
	e_direction		direction;
	int				id;
}					t_cell_data;

#endif
