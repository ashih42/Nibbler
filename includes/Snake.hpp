#ifndef SNAKE_HPP
# define SNAKE_HPP

# include "direction.hpp"
# include <vector>

class Player;
class Board;
class Cell;
class SnakeCell;

class Snake
{
public:

	Snake(Player & player, Board & board, int x, int y, e_direction direction);
	~Snake(void);

	bool										isDead(void) const;
	int											getID(void) const;
	int											getLength(void) const;
	SnakeCell &									getHeadCell(void) const;
	int											getPlayerID(void) const;
	
	void										update(void);
	void										turnLeft(void);
	void										turnRight(void);

	void										getHitAtIndex(size_t index);
	void										die(void);

private:

	Player &									_player;
	Board &										_board;
	bool										_isDead;
	e_direction									_nextDirection;
	std::vector<std::shared_ptr<SnakeCell>>		_snakeCells;

	Snake(void);
	Snake(const Snake & src);
	Snake &	operator=(const Snake & rhs);

	void										_initSnakeCells(int x, int y);
	void										_registerSnakeCellsToBoard(void);

	void										_getNextXY(int & nextX, int & nextY) const;
	void										_interactWithTarget(Cell & target);
	void										_moveAndGrow(int newX, int newY, bool isGrowing);

};

#endif
