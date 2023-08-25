CC := clang++

TARGET := Nibbler

SDL2_LOC := $(shell brew --prefix sdl2)
SDL2_INC := $(SDL2_LOC)/include/SDL2/
SDL2_LINK := -L $(SDL2_LOC)/lib/ -lSDL2

SDL2_IMAGE_LOC := $(shell brew --prefix sdl2_image)
SDL2_IMAGE_INC := $(SDL2_IMAGE_LOC)/include/SDL2/
SDL2_IMAGE_LINK := -L $(SDL2_IMAGE_LOC)/lib/ -lSDL2_image

SDL2_TTF_LOC := $(shell brew --prefix sdl2_ttf)
SDL2_TTF_INC := $(SDL2_TTF_LOC)/include/SDL2/
SDL2_TTF_LINK := -L $(SDL2_TTF_LOC)/lib/ -lSDL2_ttf

SDL2_MIXER_LOC := $(shell brew --prefix sdl2_mixer)
SDL2_MIXER_INC := $(SDL2_MIXER_LOC)/include/SDL2/
SDL2_MIXER_LINK := -L $(SDL2_MIXER_LOC)/lib/ -lSDL2_mixer

SFML_LOC := $(shell brew --prefix sfml)
SFML_INC := $(SFML_LOC)/include/
SFML_LINK := -Wl,-rpath $(SFML_LOC)/lib \
	-L $(SFML_LOC)/lib/ -lsfml-system \
	-L $(SFML_LOC)/lib/ -lsfml-graphics \
	-L $(SFML_LOC)/lib/ -lsfml-window

SFML_NETWORK_LINK := -Wl,-rpath $(SFML_LOC)/lib \
	-L $(SFML_LOC)/lib/ -lsfml-system \
	-L $(SFML_LOC)/lib/ -lsfml-network \

GLEW_LOC := $(shell brew --prefix glew)
GLEW_INC := $(GLEW_LOC)/include/
GLEW_LINK := -L $(GLEW_LOC)/lib/ -lGLEW

GLM_LOC := $(shell brew --prefix glm)
GLM_INC := $(GLM_LOC)/include/

CFLAGS := -std=c++11 -Wall -Werror -Wextra -Wfatal-errors

INCLUDES := includes/
HEADERS := -I $(INCLUDES) \
	-I $(SDL2_INC) \
	-I $(SDL2_IMAGE_INC) \
	-I $(SDL2_TTF_INC) \
	-I $(SDL2_MIXER_INC) \
	-I $(SFML_INC) \
	-I $(GLEW_INC) \
	-I $(GLM_INC)

SRCSDIR := srcs/
SRCS := \
AudioManager.cpp \
Board.cpp \
Cell.cpp \
Client.cpp \
Enemy.cpp \
EnemyCell.cpp \
FoodCell.cpp \
Model.cpp \
NetworkEntity.cpp \
Nibbler.cpp \
NibblerException.cpp \
OpenGLModule.cpp \
ParsingException.cpp \
Player.cpp \
ResourceManager.cpp \
SDLModule.cpp \
SFMLModule.cpp \
Server.cpp \
Shader.cpp \
Snake.cpp \
SnakeCell.cpp \
direction.cpp \
main.cpp

OBJDIR := objs/
OBJS := $(addprefix $(OBJDIR), $(SRCS:.cpp=.o))

MY_SFML_LIB := myLibSFML.so
MY_SDL_LIB := myLibSDL.so
MY_OPENGL_LIB := myLibOpenGL.so
MY_AUDIO_LIB := myLibAudio.so

MY_SFML_OBJS := \
objs/SFMLModule.o

MY_SDL_OBJS := \
objs/NibblerException.o \
objs/ResourceManager.o \
objs/SDLModule.o

MY_OPENGL_OBJS := \
objs/Model.o \
objs/NibblerException.o \
objs/OpenGLModule.o \
objs/ResourceManager.o \
objs/Shader.o

MY_AUDIO_OBJS := \
objs/AudioManager.o \
objs/NibblerException.o \
objs/ResourceManager.o

REMAINING_OBJS := \
objs/Board.o \
objs/Cell.o \
objs/Client.o \
objs/Enemy.o \
objs/EnemyCell.o \
objs/FoodCell.o \
objs/NetworkEntity.o \
objs/Nibbler.o \
objs/NibblerException.o \
objs/ParsingException.o \
objs/Player.o \
objs/Server.o \
objs/Snake.o \
objs/SnakeCell.o \
objs/direction.o \
objs/main.o

all: $(TARGET)

$(MY_SFML_LIB): $(MY_SFML_OBJS)
	@echo "\x1b[1mBuilding $(MY_SFML_LIB)...\x1b[0m"
	$(CC) -shared -fPIC $(HEADERS) \
		-o $(MY_SFML_LIB) $(MY_SFML_OBJS) \
		$(SFML_LINK)

$(MY_SDL_LIB): $(MY_SDL_OBJS)
	@echo "\x1b[1mBuilding $(MY_SDL_LIB)...\x1b[0m"
	$(CC) -shared -fPIC $(HEADERS) \
		-o $(MY_SDL_LIB) $(MY_SDL_OBJS) \
		$(SDL2_LINK) \
		$(SDL2_IMAGE_LINK)

$(MY_OPENGL_LIB): $(MY_OPENGL_OBJS)
	@echo "\x1b[1mBuilding $(MY_OPENGL_LIB)...\x1b[0m"
	$(CC) -shared -fPIC $(HEADERS) \
		-o $(MY_OPENGL_LIB) $(MY_OPENGL_OBJS) \
		$(SDL2_LINK) \
		$(SDL2_IMAGE_LINK) \
		$(GLEW_LINK) \
		-framework OpenGL

$(MY_AUDIO_LIB): $(MY_AUDIO_OBJS)
	@echo "\x1b[1mBuilding $(MY_AUDIO_LIB)...\x1b[0m"
	$(CC) -shared -fPIC $(HEADERS) \
		-o $(MY_AUDIO_LIB) $(MY_AUDIO_OBJS) \
		$(SDL2_LINK) \
		$(SDL2_MIXER_LINK)

$(OBJDIR)%.o: $(SRCSDIR)%.cpp
	@mkdir -p $(OBJDIR)
	$(CC) -c $(CFLAGS) $(HEADERS) $< -o $@

$(TARGET): $(OBJS) $(MY_SFML_LIB) $(MY_SDL_LIB) $(MY_OPENGL_LIB) $(MY_AUDIO_LIB)
	@echo "\x1b[1mBuilding $(TARGET)...\x1b[0m"
	$(CC) -o $(TARGET) $(REMAINING_OBJS) \
		$(MY_SFML_LIB) $(MY_SDL_LIB) $(MY_OPENGL_LIB) $(MY_AUDIO_LIB) \
		$(SFML_NETWORK_LINK)
	@echo "\x1b[1mBuild finished!!\x1b[0m"

clean:
	@echo "\x1b[1mCleaning...\x1b[0m"
	/bin/rm -rf $(OBJDIR)
	@echo

fclean: clean
	@echo "\x1b[1mFcleaning...\x1b[0m"
	/bin/rm -f $(TARGET) $(MY_SFML_LIB) $(MY_SDL_LIB) $(MY_OPENGL_LIB) $(MY_AUDIO_LIB)
	@echo

re: fclean all

.PHONY: all clean fclean re sdl2 sdl2_image sdl2_ttf sdl2_mixer sfml
