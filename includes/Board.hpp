#ifndef BOARD_HPP
# define BOARD_HPP

# include "Cell.hpp"
# include "cell_data.hpp"

# include <vector>

class FoodCell;

class Board
{
public:

	Board(int width, int height);
	~Board(void);

	int									getWidth(void) const;
	int									getHeight(void) const;
	Cell *								getCell(int x, int y) const;

	bool								isEmptyCell(int x, int y) const;
	bool								isValidPosition(int x, int y) const;

	std::vector<t_cell_data>			getCellData(void) const;

	void								setCell(const std::shared_ptr<Cell> & cell);
	void								setCell(const std::shared_ptr<Cell> & cell, int x, int y);
	void								clearCell(int x, int y);
	void								clearAllCells(void);

	FoodCell &							generateFood(void);
	void								generateFood(int id, int posX, int posY);

private:

	int									_width;
	int									_height;
	std::vector<std::shared_ptr<Cell>>	_cells;

	Board(void);
	Board(const Board & src);
	Board &	operator=(const Board & rhs);

	void								_findEmptyPosition(int & emptyX, int & emptyY);

};

#endif
