#ifndef SNAKE_CELL_HPP
# define SNAKE_CELL_HPP

# include "Cell.hpp"
# include "direction.hpp"

class Snake;

class SnakeCell : public Cell
{
public:

	SnakeCell(int x, int y, Snake & snake, e_direction direction, bool isHead, size_t index);
	virtual ~SnakeCell(void);

	Snake &					getSnake(void) const;
	e_direction				getDirection(void) const;
	bool					isHead(void) const;
	virtual t_cell_data		getCellData(void) const;

	void					setDirection(e_direction direction);
	virtual void			getHit(void);

private:

	Snake &					_snake;
	e_direction				_direction;
	bool					_isHead;
	size_t					_index;

	SnakeCell(void);
	SnakeCell(const SnakeCell & src);
	SnakeCell & operator=(const SnakeCell & rhs);

};

#endif
