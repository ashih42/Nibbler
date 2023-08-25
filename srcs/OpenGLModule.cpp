#include "OpenGLModule.hpp"
#include "ResourceManager.hpp"
#include "NibblerException.hpp"
#include "Model.hpp"
#include "Shader.hpp"

extern "C"
{
	IModule *		createOpenGLModule(int boardWidth, int boardHeight, std::string title)
	{
		return (new OpenGLModule(boardWidth, boardHeight, title));
	}
}

OpenGLModule::OpenGLModule(int boardWidth, int boardHeight, std::string title) :
	_boardWidth(boardWidth),
	_boardHeight(boardHeight)
{
	this->_initOpenGLStuff(title);
	this->_shader = new Shader();
	this->_initModels();
	this->_initLightPosition();
	this->_resetGraphicsParameters();
}

void				OpenGLModule::_initOpenGLStuff(std::string & title)
{
	if (SDL_Init(SDL_INIT_EVERYTHING))
		throw NibblerException("SDL_Init() failed");

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetSwapInterval(1);

	if (!(this->_window = SDL_CreateWindow(title.c_str(), 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL)))
		throw NibblerException("SDL_CreateWindow() failed");
	this->_context = SDL_GL_CreateContext(this->_window);

	glewExperimental = GL_TRUE;		// needed for MAC OSX

	GLenum err = glewInit();

	if (err != GLEW_OK)
		throw NibblerException("glewInit() failed:\n" +
			std::string(reinterpret_cast<const char *>(glewGetErrorString(err))));

	glEnable(GL_DEPTH_TEST);
}

void				OpenGLModule::_initModels(void)
{
	glm::mat4		tempMatrix;

	float			floorScaleX = this->_boardWidth + 2.0f;
	float			floorScaleY = this->_boardHeight + 2.0f;

	this->_floorModel = new Model("models/square.obj", "models/doge_rainbow.jpg");
	this->_floorModel->setScaleMatrix(glm::scale(glm::mat4(1.0f),
		glm::vec3(floorScaleX, floorScaleY, 1.0f)));
	this->_floorModel->setTranslationMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(-0.5f, 0.5f, -0.5f)));

	this->_wallModel = new Model("models/cube.obj", "models/wall.jpg");
	this->_wallModel->setRotationMatrix(glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
	
	this->_snakeHeadModel1 = new Model("models/finn.obj", "models/finn.png");
	this->_snakeHeadModel1->setRotationMatrix(glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)));

	this->_snakeHeadModel2 = new Model("models/cirno.obj", "models/cirno.png");
	tempMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	tempMatrix = glm::rotate(tempMatrix, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	this->_snakeHeadModel2->setRotationMatrix(tempMatrix);

	this->_snakeBodyModel1 = new Model("models/teapot.obj", "models/finn.png");
	tempMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	tempMatrix = glm::rotate(tempMatrix, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	this->_snakeBodyModel1->setRotationMatrix(tempMatrix);

	this->_snakeBodyModel2 = new Model("models/icosahedron.obj", "models/cirno.png");
	tempMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	tempMatrix = glm::rotate(tempMatrix, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	this->_snakeBodyModel2->setRotationMatrix(tempMatrix);

	this->_snakeDeadModel = new Model("models/skeleton_sit.obj", "models/skeleton.jpg");
	this->_snakeDeadModel->setRotationMatrix(glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f)));

	this->_foodModel = new Model("models/pizza.obj", "models/pizza.png");
	this->_foodModel->setSpinning();

	this->_enemyModel = new Model("models/hero_dog.obj", "models/hero_dog.png");
	this->_enemyModel->setRotationMatrix(glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
}

void				OpenGLModule::_initLightPosition(void)
{
	float			xCenter = this->_boardWidth / 2.0f;
	float			yCenter = -this->_boardHeight / 2.0f;

	int				largestSide = std::max(this->_boardWidth, this->_boardHeight);
	float			zDist = largestSide / atan(glm::radians(45.0f));
	
	zDist /= 8.0f;
	this->_lightPosition = glm::vec3(xCenter, yCenter, -zDist);
	this->_shader->setVector3("light_position", glm::value_ptr(this->_lightPosition));
}

void				OpenGLModule::_resetGraphicsParameters(void)
{
	this->_isWireframe = false;
	glPolygonMode(GL_FRONT_AND_BACK, this->_isWireframe ? GL_LINE : GL_FILL);

	this->_isShaded = false;
	this->_shader->setBool("is_shaded", this->_isShaded);

	this->_isFirstPersonView = false;
}

OpenGLModule::~OpenGLModule(void)
{
	delete this->_floorModel;
	delete this->_wallModel;
	delete this->_snakeHeadModel1;
	delete this->_snakeHeadModel2;
	delete this->_snakeBodyModel1;
	delete this->_snakeBodyModel2;
	delete this->_snakeDeadModel;
	delete this->_foodModel;
	delete this->_enemyModel;

	delete this->_shader;

	SDL_DestroyWindow(this->_window);
	SDL_GL_DeleteContext(this->_context);
	
	SDL_Quit();
}

void				OpenGLModule::disable(void)
{
	SDL_HideWindow(this->_window);
}

void				OpenGLModule::enable(void)
{
	SDL_GL_MakeCurrent(this->_window, this->_context);
	SDL_ShowWindow(this->_window);
}

std::vector<e_event>		OpenGLModule::getEvents(void)
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

void				OpenGLModule::_handleKeyPressEvent(std::vector<e_event> & myEvents, SDL_Event & event)
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
		case SDLK_KP_0:
			this->_resetGraphicsParameters();
			return;
		case SDLK_w:
			this->_toggleWireframeMode();
			return;
		case SDLK_s:
			this->_toggleShading();
			return;
		case SDLK_v:
			this->_toggleView();
			return;
		default:
			break;
	}
}
void				OpenGLModule::_toggleWireframeMode(void)
{
	this->_isWireframe = !this->_isWireframe;
	glPolygonMode(GL_FRONT_AND_BACK, this->_isWireframe ? GL_LINE : GL_FILL);
}

void				OpenGLModule::_toggleShading(void)
{
	this->_isShaded = !this->_isShaded;
	this->_shader->setBool("is_shaded", this->_isShaded);
}

void				OpenGLModule::_toggleView(void)
{
	this->_isFirstPersonView = !this->_isFirstPersonView;
}

void				OpenGLModule::_drawModelAtPosition(Model & model, int x, int y)
{
	this->_shader->setMatrix("projection_matrix", glm::value_ptr(this->_projectionMatrix));
	this->_shader->setMatrix("view_matrix", glm::value_ptr(this->_viewMatrix));

	this->_shader->setMatrix("model_scale_matrix", value_ptr(model.getScaleMatrix()));
	this->_shader->setMatrix("model_rotate_matrix", value_ptr(model.getRotationMatrix()));
	this->_shader->setMatrix("model_translate_matrix", value_ptr(
		glm::translate(glm::mat4(1.0f), glm::vec3(x, -y, 0)) * model.getTranslationMatrix()));

	model.draw();
}

void				OpenGLModule::_drawModelAtPositionFacing(Model & model, int x, int y, e_direction direction)
{
	glm::mat4		rotationMatrix;

	switch (direction)
	{
		case EAST:
			rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			break;
		case SOUTH:
			rotationMatrix = glm::mat4(1.0f);
			break;
		case WEST:
			rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			break;
		case NORTH:
			rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			break;
		default:
			rotationMatrix = glm::mat4(1.0f);
			break;
	}

	this->_shader->setMatrix("projection_matrix", glm::value_ptr(this->_projectionMatrix));
	this->_shader->setMatrix("view_matrix", glm::value_ptr(this->_viewMatrix));

	this->_shader->setMatrix("model_scale_matrix", value_ptr(model.getScaleMatrix()));
	this->_shader->setMatrix("model_rotate_matrix", value_ptr(rotationMatrix * model.getRotationMatrix()));
	this->_shader->setMatrix("model_translate_matrix", value_ptr(
		glm::translate(glm::mat4(1.0f), glm::vec3(x, -y, 0)) * model.getTranslationMatrix()));

	model.draw();
}

t_cell_data &		findActivePlayerSnakeHeadCell(std::vector<t_cell_data> & cellData)
{
	for (t_cell_data & data : cellData)
	{
		if (data.isHead && data.isActivePlayer)
			return (data);
	}
	throw NibblerException("findActivePlayerSnakeHeadCell() failed");
}

void				OpenGLModule::render(std::vector<t_cell_data> cellData)
{
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	t_cell_data &	activeHead = findActivePlayerSnakeHeadCell(cellData);

	this->_updateViewMatrix(activeHead);

	this->_renderFloorAndWalls();

	for (t_cell_data & data : cellData)
		this->_renderCell(data);

	SDL_GL_SwapWindow(this->_window);
}

void				OpenGLModule::_updateViewMatrix(t_cell_data & activeHead)
{
	static int		largestSide = std::max(this->_boardWidth, this->_boardHeight);
	static float	xCenter = this->_boardWidth / 2.0f - 0.5f;
	static float	yCenter = -(this->_boardHeight / 2.0f - 0.5f);

	static float	zDist = largestSide / atan(glm::radians(45.0f));

	// lock on player 1's view point
	if (this->_isFirstPersonView)
	{
		int			x = activeHead.posX;
		int			y = activeHead.posY;
		int			offset = 4;
		glm::mat4	rotationMatrix;

		switch (activeHead.direction)
		{
			case EAST:
				rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
				x = x - offset;
				break;
			case SOUTH:
				rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
				y = y - offset;
				break;
			case WEST:
				rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
				x = x + offset;
				break;
			case NORTH:
				rotationMatrix = glm::mat4(1.0f);
				y = y + offset;
				break;
			default:
				rotationMatrix = glm::mat4(1.0f);
				break;
		}
		this->_viewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-x, y, -3));
		this->_viewMatrix = rotationMatrix * this->_viewMatrix;

		rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(-45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		this->_viewMatrix = rotationMatrix * this->_viewMatrix;
		this->_projectionMatrix = glm::perspective(glm::radians(80.0f), ASPECT_RATIO, 0.1f, 100.0f);
	}
	else
	{
		this->_viewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-xCenter, -yCenter, -zDist));
		this->_projectionMatrix = glm::perspective(glm::radians(45.0f), ASPECT_RATIO, 0.1f, 100.0f);
	}
}

void				OpenGLModule::_renderFloorAndWalls(void)
{
	static float	floorX = this->_boardWidth / 2.0f;
	static float	floorY = this->_boardHeight / 2.0f;

	// draw floor
	this->_drawModelAtPosition(*this->_floorModel, floorX, floorY);

	// draw walls
	for (int x = -1; x <= this->_boardWidth; x++)
	{
		this->_drawModelAtPosition(*this->_wallModel, x, -1);
		this->_drawModelAtPosition(*this->_wallModel, x, this->_boardHeight);
	}
	for (int y = 0; y < this->_boardHeight; y++)
	{
		this->_drawModelAtPosition(*this->_wallModel, -1, y);
		this->_drawModelAtPosition(*this->_wallModel, this->_boardWidth, y);
	}
}


void				OpenGLModule::_renderCell(t_cell_data & cellData)
{
	switch (cellData.type)
	{
		case CELL_SNAKE:
			this->_renderSnakeCell(cellData);
			break;
		case CELL_FOOD:
			this->_renderFoodCell(cellData);
			break;
		case CELL_ENEMY:
			this->_renderEnemyCell(cellData);
			break;
		default:
			break;
	}
}

void				OpenGLModule::_renderSnakeCell(t_cell_data & cellData)
{
	Model *			model;

	// Player dieded
	if (cellData.isDead && cellData.isHead)
		model = this->_snakeDeadModel;
	// Player 1
	else if (cellData.isPlayer0)
	{
		if (cellData.isHead)
			model = this->_snakeHeadModel1;
		else
			model = this->_snakeBodyModel1;
	}
	// Player 2
	else
	{
		if (cellData.isHead)
			model = this->_snakeHeadModel2;
		else
			model = this->_snakeBodyModel2;
	}
	this->_drawModelAtPositionFacing(*model, cellData.posX, cellData.posY, cellData.direction);
}

void				OpenGLModule::_renderFoodCell(t_cell_data & cellData)
{
	this->_drawModelAtPosition(*this->_foodModel, cellData.posX, cellData.posY);
}

void				OpenGLModule::_renderEnemyCell(t_cell_data & cellData)
{
	this->_drawModelAtPositionFacing(*this->_enemyModel, cellData.posX, cellData.posY, cellData.direction);
}

