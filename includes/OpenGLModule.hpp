#ifndef OPEN_GL_MODULE_HPP
# define OPEN_GL_MODULE_HPP

# define SCREEN_WIDTH		800
# define SCREEN_HEIGHT		800
# define ASPECT_RATIO		((float)SCREEN_WIDTH / SCREEN_HEIGHT)

# include "IModule.hpp"
# include "direction.hpp"

# include <string>
# include <GL/glew.h>
# include <SDL.h>
# include <SDL_image.h>
# include <glm/glm.hpp>
# include <glm/gtc/matrix_transform.hpp>
# include <glm/gtc/type_ptr.hpp>
# define GLM_ENABLE_EXPERIMENTAL
# include <glm/gtx/string_cast.hpp>

class Shader;
class Model;

class OpenGLModule : public IModule
{
public:

	OpenGLModule(int boardWidth, int boardHeight, std::string title);
	~OpenGLModule(void);

	virtual void					disable(void);
	virtual void					enable(void);

	virtual std::vector<e_event>	getEvents(void);
	virtual void					render(std::vector<t_cell_data> cellData);

private:

	int								_boardWidth;
	int								_boardHeight;

	SDL_Window *					_window;
	SDL_GLContext					_context;
	
	Shader *						_shader;

	Model *							_floorModel;
	Model *							_wallModel;
	Model *							_snakeHeadModel1;
	Model *							_snakeHeadModel2;
	Model *							_snakeBodyModel1;
	Model *							_snakeBodyModel2;
	Model *							_snakeDeadModel;
	Model *							_foodModel;
	Model *							_enemyModel;

	glm::mat4						_transformMatrix;
	glm::mat4						_viewMatrix;
	glm::mat4						_projectionMatrix;

	glm::vec3						_lightPosition;

	bool							_isWireframe;
	bool							_isShaded;
	bool							_isFirstPersonView;

	OpenGLModule(void);
	OpenGLModule(const OpenGLModule & src);
	OpenGLModule & operator=(const OpenGLModule & rhs);

	void							_initOpenGLStuff(std::string & title);
	void							_initModels(void);
	void							_initLightPosition(void);

	void							_handleKeyPressEvent(std::vector<e_event> & myEvents, SDL_Event & event);

	void							_resetGraphicsParameters(void);
	void							_toggleWireframeMode(void);
	void							_toggleShading(void);
	void							_toggleView(void);

	void							_renderFloorAndWalls(void);
	void							_renderCell(t_cell_data & cellData);
	void							_renderSnakeCell(t_cell_data & cellData);
	void							_renderFoodCell(t_cell_data & cellData);
	void							_renderEnemyCell(t_cell_data & cellData);

	void							_drawModelAtPosition(Model & model, int x, int y);
	void							_drawModelAtPositionFacing(Model & model, int x, int y, e_direction direction);

	void							_updateViewMatrix(t_cell_data & activeHead);

};

#endif
