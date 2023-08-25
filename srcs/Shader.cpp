#include "Shader.hpp"
#include "ResourceManager.hpp"
#include "NibblerException.hpp"

# define VERTEX_SHADER_FILE		"shaders/vertex_shader.glsl"
# define FRAGMENT_SHADER_FILE	"shaders/fragment_shader.glsl"

Shader::Shader(void)
{
	std::string			vertexShaderCode = this->_readShaderFile(VERTEX_SHADER_FILE);
	std::string			fragmentShaderCode = this->_readShaderFile(FRAGMENT_SHADER_FILE);

	unsigned int		vertexShaderID = this->_compileShaderCode(GL_VERTEX_SHADER, vertexShaderCode.c_str());
	unsigned int		fragmentShaderID = this->_compileShaderCode(GL_FRAGMENT_SHADER, fragmentShaderCode.c_str());
	
	this->_programID = this->_linkShaderProgram(vertexShaderID, fragmentShaderID);
	glUseProgram(this->_programID);
	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);
}

std::string				Shader::_readShaderFile(std::string filename)
{
	std::string			shaderCode;
	std::ifstream		shaderFile;
	std::stringstream	shaderStream;

	shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		shaderFile.open(ResourceManager::getInstance().getBasePath(filename));
		shaderStream << shaderFile.rdbuf();
		shaderCode = shaderStream.str();
	}
	catch (std::ifstream::failure e)
	{
		throw NibblerException("Shader::_readShaderFile() failed on " + filename);
	}
	return (shaderCode);
}


unsigned int			Shader::_compileShaderCode(GLenum shaderType, const char * shaderCode)
{
	unsigned int		shaderID;
	int					success;
	char				infoLog[LOG_SIZE];

	shaderID = glCreateShader(shaderType);
	glShaderSource(shaderID, 1, &shaderCode, NULL);
	glCompileShader(shaderID);

	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(shaderID, LOG_SIZE, NULL, infoLog);
		throw NibblerException(std::string("glCompileShader() failed:\n") + std::string(infoLog));
	}
	return (shaderID);
}

unsigned int			Shader::_linkShaderProgram(unsigned int vertexShader, unsigned int fragmentShader)
{
	unsigned int		programID;
	int					success;
	char				infoLog[LOG_SIZE];

	programID = glCreateProgram();
	glAttachShader(programID, vertexShader);
	glAttachShader(programID, fragmentShader);
	glLinkProgram(programID);

	glGetProgramiv(programID, GL_LINK_STATUS, &success);
	if (!success)
	{
	    glGetProgramInfoLog(programID, LOG_SIZE, NULL, infoLog);
	    throw NibblerException(std::string("glLinkProgram() failed:\n") + std::string(infoLog));
	}
	return (programID);
}

Shader::~Shader(void)
{
	glDeleteProgram(this->_programID);
}

void					Shader::setMatrix(std::string name, const float * matrix)
{
	unsigned int		matrixLoc = glGetUniformLocation(this->_programID, name.c_str());

	glUniformMatrix4fv(matrixLoc, 1, GL_FALSE, matrix);
}

void					Shader::setVector3(std::string name, float * vector)
{
	unsigned int		vectorLoc = glGetUniformLocation(this->_programID, name.c_str());

	glUniform3fv(vectorLoc, 1, vector);
}

void					Shader::setBool(std::string name, bool value)
{
	unsigned int		loc = glGetUniformLocation(this->_programID, name.c_str());

	glUniform1i(loc, static_cast<int>(value));
}
