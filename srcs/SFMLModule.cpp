#include "SFMLModule.hpp"

# define CELL_WIDTH		40

extern "C"
{
	IModule *				createSFMLModule(int boardWidth, int boardHeight, std::string title)
	{
		return (new SFMLModule(boardWidth, boardHeight, title));
	}
}

SFMLModule::SFMLModule(int boardWidth, int boardHeight, std::string title) :
	_boardWidth(boardWidth),
	_boardHeight(boardHeight),
	_title(title)
{
	this->enable();
}

SFMLModule::~SFMLModule(void)
{
	this->_renderWindow.close();
}

void						SFMLModule::disable(void)
{
	this->_renderWindow.close();
}

void						SFMLModule::enable(void)
{
	this->_renderWindow.create(sf::VideoMode(this->_boardWidth * CELL_WIDTH, this->_boardHeight * CELL_WIDTH), this->_title);
	this->_renderWindow.setPosition(sf::Vector2i(0, 0));
	this->_renderWindow.setKeyRepeatEnabled(false);
}

std::vector<e_event>		SFMLModule::getEvents(void)
{
	std::vector<e_event>	myEvents;
	sf::Event				event;

	while (this->_renderWindow.pollEvent(event))
	{
		if (event.type == sf::Event::EventType::Closed)
			myEvents.push_back(EVENT_TERMINATE);
		else if (event.type == sf::Event::EventType::KeyPressed)
			this->_handleKeyPressEvent(myEvents, event);
	}
	return (myEvents);
}

void						SFMLModule::_handleKeyPressEvent(std::vector<e_event> & myEvents, sf::Event & event)
{
	switch (event.key.code)
	{
		// Gameplay Controls
		case sf::Keyboard::Escape:
			myEvents.push_back(EVENT_TERMINATE);
			break;
		case sf::Keyboard::Left:
			myEvents.push_back(EVENT_P1_TURN_LEFT);
			break;
		case sf::Keyboard::Right:
			myEvents.push_back(EVENT_P1_TURN_RIGHT);
			break;
		case sf::Keyboard::Numpad4:
			myEvents.push_back(EVENT_P2_TURN_LEFT);
			break;
		case sf::Keyboard::Numpad6:
			myEvents.push_back(EVENT_P2_TURN_RIGHT);
			break;
		case sf::Keyboard::R:
			myEvents.push_back(EVENT_START_NEW_ROUND);
			break;
		// Graphics Controls
		case sf::Keyboard::Num1:
			myEvents.push_back(EVENT_SELECT_MODULE_1);
			break;
		case sf::Keyboard::Num2:
			myEvents.push_back(EVENT_SELECT_MODULE_2);
			break;
		case sf::Keyboard::Num3:
			myEvents.push_back(EVENT_SELECT_MODULE_3);
			break;
		default:
			break;
	}
}

void						SFMLModule::render(std::vector<t_cell_data> cellData)
{
	this->_renderWindow.clear(sf::Color::Black);

	for (t_cell_data & data : cellData)
		this->_renderCell(data);

	this->_renderWindow.display();
}

void						SFMLModule::_renderCell(t_cell_data & cellData)
{
	sf::RectangleShape		rect;

	rect.setSize(sf::Vector2f(CELL_WIDTH, CELL_WIDTH));
	rect.setPosition(cellData.posX * CELL_WIDTH, cellData.posY * CELL_WIDTH);

	switch (cellData.type)
	{
		case CELL_SNAKE:
			this->_renderSnakeCell(cellData, rect);
			break;
		case CELL_FOOD:
			this->_renderFoodCell(cellData, rect);
			break;
		case CELL_ENEMY:
			this->_renderEnemyCell(cellData, rect);
			break;
		default:
			break;
	}
}

void						SFMLModule::_renderSnakeCell(t_cell_data & cellData, sf::RectangleShape & rect)
{
	// Player dieded
	if (cellData.isDead && cellData.isHead)
		rect.setFillColor(sf::Color(128, 128, 128));	// Grey
	// Player 1
	else if (cellData.isPlayer0)
	{
		rect.setFillColor(sf::Color::Cyan);
		if (cellData.isHead)
		{
			rect.setOutlineColor(sf::Color::Blue);
			rect.setOutlineThickness(20);
		}
	}
	// Player 2
	else
	{
		rect.setFillColor(sf::Color::Magenta);
		if (cellData.isHead)
		{
			rect.setOutlineColor(sf::Color::Red);
			rect.setOutlineThickness(20);
		}
	}
	this->_renderWindow.draw(rect);
}

void						SFMLModule::_renderFoodCell(t_cell_data & cellData, sf::RectangleShape & rect)
{
	(void)cellData;

	rect.setFillColor(sf::Color::Yellow);
	this->_renderWindow.draw(rect);
}

void						SFMLModule::_renderEnemyCell(t_cell_data & cellData, sf::RectangleShape & rect)
{
	(void)cellData;

	rect.setFillColor(sf::Color(255, 165, 0));	// orange
	this->_renderWindow.draw(rect);
}

