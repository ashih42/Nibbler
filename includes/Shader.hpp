#ifndef SHADER_HPP
# define SHADER_HPP

# define LOG_SIZE		512

# include <string>
# include <fstream>
# include <sstream>
# include <GL/glew.h>

class Shader
{
public:

	Shader(void);
	~Shader(void);

	void				setMatrix(std::string name, const float * matrix);
	void				setVector3(std::string name, float * vector);
	void				setBool(std::string name, bool value);

private:

	unsigned int		_programID;

	Shader(const Shader & src);
	Shader & operator=(const Shader & rhs);

	std::string			_readShaderFile(std::string filename);
	unsigned int		_compileShaderCode(GLenum shaderType, const char * shaderCode);
	unsigned int		_linkShaderProgram(unsigned int vertexShader, unsigned int fragmentShader);

};

#endif
