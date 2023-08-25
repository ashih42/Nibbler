#include "SDLModule.hpp"
#include "ResourceManager.hpp"
#include "NibblerException.hpp"

# define CELL_WIDTH			24

extern "C"
{
	IModule *		createSDLModule(int boardWidth, int boardHeight, std::string title)
	{
		return (new SDLModule(boardWidth, boardHeight, title));
	}
}

SDLModule::SDLModule(int boardWidth, int boardHeight, std::string title) :
	_boardWidth(boardWidth),
	_boardHeight(boardHeight),
	_isGridShown(false)
{
	if (SDL_Init(SDL_INIT_EVERYTHING))
		throw NibblerException("SDL_Init() failed");

	if (!(this->_window = SDL_CreateWindow(title.c_str(), 0, 0, this->_boardWidth * CELL_WIDTH, this->_boardHeight * CELL_WIDTH, SDL_WINDOW_SHOWN)))
		throw NibblerException("SDL_CreateWindow() failed");
	this->_context = SDL_GL_CreateContext(this->_window);

	if (!(this->_renderer = SDL_CreateRenderer(this->_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC)))
		throw NibblerException("SDL_CreateRenderer() failed");

	if ((IMG_Init(IMG_FLAGS) & IMG_FLAGS) != IMG_FLAGS)
		throw NibblerException("IMG_Init() failed");

	this->_snakeHeadP1Texture = this->_initTexture(SNAKE_HEAD_P1_IMAGE);
	this->_snakeBodyP1Texture = this->_initTexture(SNAKE_BODY_P1_IMAGE);
	this->_snakeHeadP2Texture = this->_initTexture(SNAKE_HEAD_P2_IMAGE);
	this->_snakeBodyP2Texture = this->_initTexture(SNAKE_BODY_P2_IMAGE);
	this->_snakeDeadTexture = this->_initTexture(SNAKE_DEAD_IMAGE);	

	this->_foodTextures.push_back(this->_initTexture(FOOD_IMAGE_0));
	this->_foodTextures.push_back(this->_initTexture(FOOD_IMAGE_1));
	this->_foodTextures.push_back(this->_initTexture(FOOD_IMAGE_2));
	this->_foodTextures.push_back(this->_initTexture(FOOD_IMAGE_3));
	this->_foodTextures.push_back(this->_initTexture(FOOD_IMAGE_4));

	this->_enemyTextures.push_back(this->_initTexture(ENEMY_IMAGE_0));
	this->_enemyTextures.push_back(this->_initTexture(ENEMY_IMAGE_1));
	this->_enemyTextures.push_back(this->_initTexture(ENEMY_IMAGE_2));
	this->_enemyTextures.push_back(this->_initTexture(ENEMY_IMAGE_3));
	this->_enemyTextures.push_back(this->_initTexture(ENEMY_IMAGE_4));
	this->_enemyTextures.push_back(this->_initTexture(ENEMY_IMAGE_5));
	this->_enemyTextures.push_back(this->_initTexture(ENEMY_IMAGE_6));
	this->_enemyTextures.push_back(this->_initTexture(ENEMY_IMAGE_7));
}

SDL_Texture *		SDLModule::_initTexture(std::string filename)
{
	SDL_Texture *	texture = IMG_LoadTexture(this->_renderer, ResourceManager::getInstance().getBasePath(filename).c_str());

	if (!texture)
		throw NibblerException("IMG_LoadTexture() failed on \'" + filename + "\'");
	return (texture);
}

SDLModule::~SDLModule(void)
{
	SDL_DestroyRenderer(this->_renderer);
	SDL_DestroyWindow(this->_window);
	SDL_GL_DeleteContext(this->_context);

	SDL_DestroyTexture(this->_snakeHeadP1Texture);
	SDL_DestroyTexture(this->_snakeBodyP1Texture);
	SDL_DestroyTexture(this->_snakeHeadP2Texture);
	SDL_DestroyTexture(this->_snakeBodyP2Texture);
	SDL_DestroyTexture(this->_snakeDeadTexture);

	for (SDL_Texture * texture : this->_foodTextures)
		SDL_DestroyTexture(texture);
	for (SDL_Texture * texture : this->_enemyTextures)
		SDL_DestroyTexture(texture);

	IMG_Quit();
	SDL_Quit();
}

void				SDLModule::disable(void)
{
	SDL_HideWindow(this->_window);
}

void				SDLModule::enable(void)
{
	SDL_GL_MakeCurrent(this->_window, this->_context);
	SDL_ShowWindow(this->_window);
}

std::vector<e_event>		SDLModule::getEvents(void)
{
	std::vector<e_event>	myEvents;
	SDL_Event				event;

	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT)
			myEvents.push_back(EVENT_TERMINATE);
		else if (event.type == SDL_KEYDOWN && event.key.repeat == 0)
			this->_handleKeyPressEvent(myEvents, event);
	}

	return (myEvents);
}

void				SDLModule::_handleKeyPressEvent(std::vector<e_event> & myEvents, SDL_Event & event)
{
	switch (event.key.keysym.sym)
	{
		// Gameplay Controls
		case SDLK_ESCAPE:
			myEvents.push_back(EVENT_TERMINATE);
			break;
		case SDLK_LEFT:
			myEvents.push_back(EVENT_P1_TURN_LEFT);
			break;
		case SDLK_RIGHT:
			myEvents.push_back(EVENT_P1_TURN_RIGHT);
			break;
		case SDLK_KP_4:
			myEvents.push_back(EVENT_P2_TURN_LEFT);
			break;
		case SDLK_KP_6:
			myEvents.push_back(EVENT_P2_TURN_RIGHT);
			break;
		case SDLK_r:
			myEvents.push_back(EVENT_START_NEW_ROUND);
			break;
		// Graphics Controls
		case SDLK_1:
			myEvents.push_back(EVENT_SELECT_MODULE_1);
			break;
		case SDLK_2:
			myEvents.push_back(EVENT_SELECT_MODULE_2);
			break;
		case SDLK_3:
			myEvents.push_back(EVENT_SELECT_MODULE_3);
			break;
		case SDLK_g:
			this->_toggleGrid();
			break;
		default:
			break;
	}
}

void				SDLModule::_toggleGrid(void)
{
	this->_isGridShown = !this->_isGridShown;
}

void				SDLModule::_drawGrid(void)
{
	SDL_SetRenderDrawColor(this->_renderer, 255, 255, 255, 0);
	// draw vertical lines
	int verticalLength = this->_boardHeight * CELL_WIDTH;
	for (int x = 0; x < this->_boardWidth; x++)
	{
		int xPosition = x * CELL_WIDTH;
		SDL_RenderDrawLine(this->_renderer, xPosition, 0, xPosition, verticalLength);
	}
	// draw horizontal lines
	int horizontalLength = this->_boardWidth * CELL_WIDTH;
	for (int y = 0; y < this->_boardHeight; y++)
	{
		int yPosition = y * CELL_WIDTH;
		SDL_RenderDrawLine(this->_renderer, 0, yPosition, horizontalLength, yPosition);
	}
}

void				SDLModule::render(std::vector<t_cell_data> cellData)
{
	SDL_SetRenderDrawColor(this->_renderer, 0, 0, 0, 0);
	SDL_RenderClear(this->_renderer);
	
	if (this->_isGridShown)
		this->_drawGrid();

	for (t_cell_data & data : cellData)
		this->_renderCell(data);

	SDL_RenderPresent(this->_renderer);
}

void				SDLModule::_renderCell(t_cell_data & cellData)
{
	SDL_Rect		dstRect;

	dstRect.x = cellData.posX * CELL_WIDTH;
	dstRect.y = cellData.posY * CELL_WIDTH;
	dstRect.w = CELL_WIDTH;
	dstRect.h = CELL_WIDTH;

	switch (cellData.type)
	{
		case CELL_SNAKE:
			this->_renderSnakeCell(cellData, dstRect);
			break;
		case CELL_FOOD:
			this->_renderFoodCell(cellData, dstRect);
			break;
		case CELL_ENEMY:
			this->_renderEnemyCell(cellData, dstRect);
			break;
		default:
			break;
	}
}

void				SDLModule::_renderSnakeCell(t_cell_data & cellData, SDL_Rect & dstRect)
{
	SDL_Texture *	texture;
	double			angle;

	switch (cellData.direction)
	{
		case NORTH:
			angle = 0.0;
			break;
		case EAST:
			angle = 90.0;
			break;
		case SOUTH:
			angle = 180.0;
			break;
		case WEST:
			angle = 270.0;
			break;
		default:
			angle = 0.0;
			break;
	}
	// Player dieded
	if (cellData.isDead && cellData.isHead)
	{
		texture = this->_snakeDeadTexture;
	}
	// Player 1
	else if (cellData.isPlayer0)
	{
		if (cellData.isHead)
			texture = this->_snakeHeadP1Texture;
		else
			texture = this->_snakeBodyP1Texture;
	}
	// Player 2
	else
	{
		if (cellData.isHead)
			texture = this->_snakeHeadP2Texture;
		else
			texture = this->_snakeBodyP2Texture;
	}
	SDL_RenderCopyEx(this->_renderer, texture, NULL, &dstRect, angle, NULL, SDL_FLIP_NONE);
}

void				SDLModule::_renderFoodCell(t_cell_data & cellData, SDL_Rect & dstRect)
{
	SDL_Texture *	texture = this->_foodTextures[cellData.id % this->_foodTextures.size()];

	SDL_RenderCopy(this->_renderer, texture, NULL, &dstRect);
}

void				SDLModule::_renderEnemyCell(t_cell_data & cellData, SDL_Rect & dstRect)
{
	SDL_Texture *	texture = this->_enemyTextures[cellData.id % this->_enemyTextures.size()];

	SDL_RenderCopy(this->_renderer, texture, NULL, &dstRect);
}

