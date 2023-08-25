#ifndef SDL_MODULE_HPP
# define SDL_MODULE_HPP

# include "IModule.hpp"

# include <string>
# include <vector>
# include <SDL.h>
# include <SDL_image.h>

# define IMG_FLAGS					(IMG_INIT_JPG | IMG_INIT_PNG)

# define SNAKE_HEAD_P1_IMAGE		"images/snake/woman_shrug_N.png"
# define SNAKE_BODY_P1_IMAGE		"images/snake/dress_N.png"

# define SNAKE_HEAD_P2_IMAGE		"images/snake/woman_ok_N.png"
# define SNAKE_BODY_P2_IMAGE		"images/snake/kimono_N.png"

# define SNAKE_DEAD_IMAGE			"images/snake/skull_N.png"

# define FOOD_IMAGE_0				"images/food/burger.png"
# define FOOD_IMAGE_1				"images/food/burrito.png"
# define FOOD_IMAGE_2				"images/food/hot_dog.png"
# define FOOD_IMAGE_3				"images/food/pizza.png"
# define FOOD_IMAGE_4				"images/food/taco.png"

# define ENEMY_IMAGE_0				"images/enemy/cat.png"
# define ENEMY_IMAGE_1				"images/enemy/chick.png"
# define ENEMY_IMAGE_2				"images/enemy/chicken.png"
# define ENEMY_IMAGE_3				"images/enemy/cow.png"
# define ENEMY_IMAGE_4				"images/enemy/crab.png"
# define ENEMY_IMAGE_5				"images/enemy/dog.png"
# define ENEMY_IMAGE_6				"images/enemy/duck.png"
# define ENEMY_IMAGE_7				"images/enemy/pig.png"

class SDLModule : public IModule
{
public:

	SDLModule(int boardWidth, int boardHeight, std::string title);
	~SDLModule(void);

	virtual void					disable(void);
	virtual void					enable(void);

	virtual std::vector<e_event>	getEvents(void);
	virtual void					render(std::vector<t_cell_data> cellData);

private:

	int								_boardWidth;
	int								_boardHeight;

	bool							_isGridShown;

	SDL_Window *					_window;
	SDL_Renderer *					_renderer;
	SDL_GLContext					_context;

	SDL_Texture *					_snakeHeadP1Texture;
	SDL_Texture *					_snakeBodyP1Texture;
	SDL_Texture *					_snakeHeadP2Texture;
	SDL_Texture *					_snakeBodyP2Texture;
	SDL_Texture *					_snakeDeadTexture;

	std::vector<SDL_Texture *>		_foodTextures;
	std::vector<SDL_Texture *>		_enemyTextures;

	SDLModule(void);
	SDLModule(const SDLModule & src);
	SDLModule &	operator=(const SDLModule & rhs);

	SDL_Texture *					_initTexture(std::string filename);

	void							_handleKeyPressEvent(std::vector<e_event> & myEvents, SDL_Event & event);

	void							_toggleGrid(void);
	void							_drawGrid(void);

	void							_renderCell(t_cell_data & cellData);
	void							_renderSnakeCell(t_cell_data & cellData, SDL_Rect & dstRect);
	void							_renderFoodCell(t_cell_data & cellData, SDL_Rect & dstRect);
	void							_renderEnemyCell(t_cell_data & cellData, SDL_Rect & dstRect);

};

#endif
