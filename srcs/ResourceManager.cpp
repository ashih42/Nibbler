#include "ResourceManager.hpp"
#include "NibblerException.hpp"
#include <SDL.h>

ResourceManager &	ResourceManager::getInstance(void)
{
	static ResourceManager instance;

	return (instance);
}

ResourceManager::ResourceManager(void)
{
	char * basePath = SDL_GetBasePath();	// does NOT depend on SDL_Init()

	if (basePath)
	{
		this->_basePath = std::string(basePath);
		SDL_free(basePath);
	}
	else
		throw NibblerException("SDL_GetBasePath() failed");
}

ResourceManager::~ResourceManager(void)
{
	
}

std::string			ResourceManager::getBasePath(std::string filename)
{
	return (this->_basePath + filename);
}
