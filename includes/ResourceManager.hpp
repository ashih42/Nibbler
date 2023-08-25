#ifndef RESOURCE_MANAGER_HPP
# define RESOURCE_MANAGER_HPP

# include <string>

class ResourceManager
{
public:

	~ResourceManager(void);

	static ResourceManager &	getInstance(void);
	
	std::string					getBasePath(std::string filename);

private:

	std::string					_basePath;

	ResourceManager(void);
	ResourceManager(const ResourceManager & src);
	ResourceManager & operator=(const ResourceManager & rhs);

};

#endif
