#ifndef SFML_MODULE_HPP
# define SFML_MODULE_HPP

# include "IModule.hpp"

# include <string>
# include <SFML/Graphics.hpp>

class SFMLModule : public IModule
{
public:

	SFMLModule(int boardWidth, int boardHeight, std::string title);
	~SFMLModule(void);

	virtual void					disable(void);
	virtual void					enable(void);

	virtual std::vector<e_event>	getEvents(void);
	virtual void					render(std::vector<t_cell_data> cellData);

private:

	int								_boardWidth;
	int								_boardHeight;
	std::string						_title;

	sf::RenderWindow				_renderWindow;

	SFMLModule(void);
	SFMLModule(const SFMLModule & src);
	SFMLModule & operator=(const SFMLModule & rhs);

	void							_handleKeyPressEvent(std::vector<e_event> & myEvents, sf::Event & event);

	void							_renderCell(t_cell_data & cellData);
	void							_renderSnakeCell(t_cell_data & cellData, sf::RectangleShape & rect);
	void							_renderFoodCell(t_cell_data & cellData, sf::RectangleShape & rect);
	void							_renderEnemyCell(t_cell_data & cellData, sf::RectangleShape & rect);

};

#endif
