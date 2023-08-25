#include "Model.hpp"
#include "ResourceManager.hpp"
#include "NibblerException.hpp"

#include <cfloat>
#include <iostream>

Model::Model(std::string objFilename, std::string textureFilename) :
	_objFilename(objFilename),
	_line_number(0),
	_triangle_count(0),
	_scaleMatrix(glm::mat4(1.0f)),
	_rotationMatrix(glm::mat4(1.0f)),
	_translationMatrix(glm::mat4(1.0f)),
	_isSpinning(false),
	_xMin(FLT_MAX), _xMax(-FLT_MAX),
	_yMin(FLT_MAX), _yMax(-FLT_MAX),
	_zMin(FLT_MAX), _zMax(-FLT_MAX)
{
	this->_readOBJFile(objFilename);
	this->_centerAndScaleModel();
	this->_generateGLData();
	this->_initVertexObjects();
	this->_initTexture(textureFilename);
}

Model::~Model(void)
{

}

const glm::mat4 &		Model::getScaleMatrix(void) const
{
	return (this->_scaleMatrix);
}

const glm::mat4 &		Model::getRotationMatrix(void) const
{
	return (this->_rotationMatrix);
}

const glm::mat4 &		Model::getTranslationMatrix(void) const
{
	return (this->_translationMatrix);
}

void					Model::setScaleMatrix(glm::mat4 matrix)
{
	this->_scaleMatrix = matrix;
}

void					Model::setRotationMatrix(glm::mat4 matrix)
{
	this->_rotationMatrix = matrix;
}

void					Model::setTranslationMatrix(glm::mat4 matrix)
{
	this->_translationMatrix = matrix;
}

void					Model::setSpinning(void)
{
	this->_isSpinning = true;
}

void					Model::draw(void)
{
	if (this->_isSpinning)
		this->_spin();

	glBindTexture(GL_TEXTURE_2D, this->_textureID);
	glBindVertexArray(this->_vao);
	glDrawArrays(GL_TRIANGLES, 0, 3 * this->_triangle_count);
}

# define SPIN_INC		2.0f

void					Model::_spin(void)
{
	static bool			xSpinDirection = 0;
	static bool			ySpinDirection = 0;
	static bool			zSpinDirection = 0;

	static float		degreesAroundX = 0.0f;
	static float		degreesAroundY = 0.0f;
	static float		degreesAroundZ = 0.0f;

	if (std::rand() % 1000 == 1)
		xSpinDirection = ! xSpinDirection;
	if (std::rand() % 1000 == 1)
		ySpinDirection = ! ySpinDirection;
	if (std::rand() % 1000 == 1)
		zSpinDirection = ! zSpinDirection;

	degreesAroundX += (xSpinDirection) ? SPIN_INC : -SPIN_INC;
	degreesAroundY += (ySpinDirection) ? SPIN_INC : -SPIN_INC;
	degreesAroundZ += (zSpinDirection) ? SPIN_INC : -SPIN_INC;

	this->_rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(degreesAroundX), glm::vec3(1.0f, 0.0f, 0.0f));
	this->_rotationMatrix = glm::rotate(this->_rotationMatrix, glm::radians(degreesAroundY), glm::vec3(0.0f, 1.0f, 0.0f));
	this->_rotationMatrix = glm::rotate(this->_rotationMatrix, glm::radians(degreesAroundZ), glm::vec3(0.0f, 0.0f, 1.0f));
}

void					Model::_readOBJFile(std::string & filename)
{
	std::ifstream		objFile;
	std::stringstream	objStream;
	std::string			line;

	objFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		objFile.open(ResourceManager::getInstance().getBasePath(filename));
		objStream << objFile.rdbuf();
	}
	catch (std::ifstream::failure e)
	{
		throw NibblerException("Model::_readOBJFile() failed on " + filename);
	}
	while (std::getline(objStream, line))
	{
		this->_parseOBJLine(line);
	}
}

void					Model::_parseOBJLine(std::string & line)
{
	try
	{
		this->_line_number++;
		if (strncmp(line.c_str(), "v ", 2) == 0)
			this->_handleVLine(line);
		else if (strncmp(line.c_str(), "vt ", 3) == 0)
			this->_handleVTLine(line);
		else if (strncmp(line.c_str(), "vn ", 3) == 0)
			this->_handleVNLine(line);
		else if (strncmp(line.c_str(), "f ", 2) == 0)
			this->_handleFLine(line);
	}
	catch (std::exception & exception)
	{
		throw NibblerException("Model::_parseOBJLine() failed:\nInvalid line in "
			+ this->_objFilename + " line " + std::to_string(this->_line_number) + ":\n" + line);
	}
}

/*
** Possible formats:
** v x y z
** v x y z w
*/
void					Model::_handleVLine(std::string line)
{
	std::vector<std::string>	tokens;
	std::istringstream			iss(line);
	t_vec3						v;
	
	for (std::string token; iss >> token; )
		tokens.push_back(token);

	if (!(tokens.size() == 4 || tokens.size() == 5))
		throw NibblerException("u w0t m8");

	v.x = std::stof(tokens[1]);
	v.y = std::stof(tokens[2]);
	v.z = std::stof(tokens[3]);
	this->_vData.push_back(v);

	if (v.x < this->_xMin)
		this->_xMin = v.x;
	if (v.x > this->_xMax)
		this->_xMax = v.x;

	if (v.y < this->_yMin)
		this->_yMin = v.y;
	if (v.y > this->_yMax)
		this->_yMax = v.y;

	if (v.z < this->_zMin)
		this->_zMin = v.z;
	if (v.z > this->_zMax)
		this->_zMax = v.z;
}

/*
** Possible formats:
** vt u v
** vt u v w
*/
void					Model::_handleVTLine(std::string line)
{
	std::vector<std::string>	tokens;
	std::istringstream			iss(line);
	t_vec2						vt;
	
	for (std::string token; iss >> token; )
		tokens.push_back(token);

	if (!(tokens.size() == 3 || tokens.size() == 4))
		throw NibblerException("u w0t m8");

	vt.x = std::stof(tokens[1]);
	vt.y = std::stof(tokens[2]);
	this->_vtData.push_back(vt);
}

/*
** Possible formats:
** vn i j k
*/
void					Model::_handleVNLine(std::string line)
{
	std::vector<std::string>	tokens;
	std::istringstream			iss(line);
	t_vec3						vn;
	
	for (std::string token; iss >> token; )
		tokens.push_back(token);

	if (tokens.size() != 4)
		throw NibblerException("u w0t m8");

	vn.x = std::stof(tokens[1]);
	vn.y = std::stof(tokens[2]);
	vn.z = std::stof(tokens[3]);
	this->_vnData.push_back(vn);
}

/*
** Possible formats:
** f expr_1 expr_2 expr_3 ... expr_n
*/
void					Model::_handleFLine(std::string line)
{
	std::vector<std::string>	tokens;
	std::istringstream			iss(line);
	
	for (std::string token; iss >> token; )
		tokens.push_back(token);

	if (tokens.size() < 4)
		throw NibblerException("u w0t m8");
	
	for (size_t i = 3; i < tokens.size(); i++)
		this->_parseTriangle(tokens[1], tokens[i - 1], tokens[i]);
}

/*
** Possible formats:
** f	v1				v2				v3
** f	v1/vt1			v2/vt2			v3/vt3
** f	v1/vt1/vn1		v2/vt2/vn2		v3/vt3/vn3
** f	v1//vn1			v2//vn2			v3//vn3
*/
void					Model::_parseTriangle(
	std::string expr_1, std::string expr_2, std::string expr_3)
{
	std::string			v1_str, vt1_str, vn1_str;
	std::string			v2_str, vt2_str, vn2_str;
	std::string			v3_str, vt3_str, vn3_str;
	t_f					f;

	this->_parseVertexTerm(expr_1, v1_str, vt1_str, vn1_str);
	this->_parseVertexTerm(expr_2, v2_str, vt2_str, vn2_str);
	this->_parseVertexTerm(expr_3, v3_str, vt3_str, vn3_str);

	f.v1 = std::stoi(v1_str);
	f.v2 = std::stoi(v2_str);
	f.v3 = std::stoi(v3_str);

	f.vt1 = vt1_str == "" ? UNDEFINED_INDEX : std::stoi(vt1_str);
	f.vt2 = vt2_str == "" ? UNDEFINED_INDEX : std::stoi(vt2_str);
	f.vt3 = vt3_str == "" ? UNDEFINED_INDEX : std::stoi(vt3_str);

	f.vn1 = vn1_str == "" ? UNDEFINED_INDEX : std::stoi(vn1_str);
	f.vn2 = vn2_str == "" ? UNDEFINED_INDEX : std::stoi(vn2_str);
	f.vn3 = vn3_str == "" ? UNDEFINED_INDEX : std::stoi(vn3_str);

	this->_fData.push_back(f);
	this->_triangle_count++;
}

void					Model::_parseVertexTerm(
	std::string & expr, std::string & v_str, std::string & vt_str, std::string & vn_str)
{
	std::stringstream	exprStream(expr);
	std::string			substr;

	std::getline(exprStream, substr, '/');
	v_str = substr;

	if (std::getline(exprStream, substr, '/'))
		vt_str = substr;

	if (std::getline(exprStream, substr, '/'))
		vn_str = substr;

	if (std::getline(exprStream, substr, '/'))
		throw NibblerException("u w0t m8");
}

void					Model::_centerAndScaleModel(void)
{
	if (this->_xMin == FLT_MAX || this->_xMax == -FLT_MAX ||
		this->_yMin == FLT_MAX || this->_zMax == -FLT_MAX ||
		this->_yMin == FLT_MAX || this->_zMax == -FLT_MAX)
		throw NibblerException("Model::_centerAndScaleModel() failed on " + this->_objFilename);

	float				xRange = this->_xMax - this->_xMin;
	float				yRange = this->_yMax - this->_yMin;
	float				zRange = this->_zMax - this->_zMin;
	float				largestRange;

	largestRange = xRange > yRange ? xRange : yRange;
	largestRange = largestRange > zRange ? largestRange : zRange;
	for (s_vec3 & v : this->_vData)
	{
		v.x = (v.x - this->_xMin) - xRange / 2.0f;
		v.y = (v.y - this->_yMin) - yRange / 2.0f;
		v.z = (v.z - this->_zMin) - zRange / 2.0f;
		v.x /= largestRange;
		v.y /= largestRange;
		v.z /= largestRange;
	}
}

void					Model::_generateGLData(void)
{
	for (t_f f : this->_fData)
	{
		t_vec3 v1 = this->_getV(f.v1);
		t_vec3 v2 = this->_getV(f.v2);
		t_vec3 v3 = this->_getV(f.v3);

		t_vec2 vt1 = this->_getVT(f.vt1);
		t_vec2 vt2 = this->_getVT(f.vt2);
		t_vec2 vt3 = this->_getVT(f.vt3);

		glm::vec3 side1(v2.x - v1.x, v2.y - v1.y, v2.z - v1.z);
		glm::vec3 side2(v3.x - v1.x, v3.y - v1.y, v3.z - v1.z);
		glm::vec3 faceNormal = glm::normalize(glm::cross(side1, side2));

		t_vec3 vn1 = this->_getVN(f.vn1, faceNormal);
		t_vec3 vn2 = this->_getVN(f.vn2, faceNormal);
		t_vec3 vn3 = this->_getVN(f.vn3, faceNormal);

		this->_generateGLDataRow(v1, vt1, vn1, faceNormal);
		this->_generateGLDataRow(v2, vt2, vn2, faceNormal);
		this->_generateGLDataRow(v3, vt3, vn3, faceNormal);
	}
}

void					Model::_generateGLDataRow(t_vec3 & v, t_vec2 & vt, t_vec3 & vn, glm::vec3 & faceNormal)
{
	this->_glData.push_back(v.x);
	this->_glData.push_back(v.y);
	this->_glData.push_back(v.z);

	this->_glData.push_back(vt.x);
	this->_glData.push_back(vt.y);

	this->_glData.push_back(vn.x);
	this->_glData.push_back(vn.y);
	this->_glData.push_back(vn.z);

	this->_glData.push_back(faceNormal.x);
	this->_glData.push_back(faceNormal.y);
	this->_glData.push_back(faceNormal.z);
}

t_vec3					Model::_getV(int vIndex)
{
	if (!(1 <= vIndex && vIndex <= static_cast<int>(this->_vData.size())))
		throw NibblerException("Model::_getV() failed on invalid vIndex " + std::to_string(vIndex));
	return (this->_vData[vIndex - 1]);
}

t_vec2					Model::_getVT(int vtIndex)
{
	static t_vec2		undefinedVT = {UNDEFINED_VALUE, UNDEFINED_VALUE};

	if (vtIndex == UNDEFINED_INDEX)
		return (undefinedVT);

	if (!(1 <= vtIndex && vtIndex <= static_cast<int>(this->_vtData.size())))
		throw NibblerException("Model::_getVT() failed on invalid vtIndex " + std::to_string(vtIndex));
	return (this->_vtData[vtIndex - 1]);
}

t_vec3					Model::_getVN(int vnIndex, glm::vec3 & faceNormal)
{
	if (vnIndex == UNDEFINED_INDEX)
	{
		t_vec3 normal = {faceNormal.x, faceNormal.y, faceNormal.z};
		return (normal);
	}

	if (!(1 <= vnIndex && vnIndex <= static_cast<int>(this->_vnData.size())))
		throw NibblerException("Model::_getVN() failed on invalid vnIndex " + std::to_string(vnIndex));
	return (this->_vnData[vnIndex - 1]);
}

# define STRIDE_LEN 11

/*
** Each vertex data contains:
** position				(x, y, z)		(from .obj file)
** texture coordinates	(u, v)			(from .obj file)
** vertex normal		(x, y, z)		(from .obj file)
** face normal			(x, y, z)		(generated per triangle)
*/

void					Model::_initVertexObjects(void)
{
	glGenVertexArrays(1, &this->_vao);
	glGenBuffers(1, &this->_vbo);
	glBindVertexArray(this->_vao);
	glBindBuffer(GL_ARRAY_BUFFER, this->_vbo);
	
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * this->_glData.size(), this->_glData.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, STRIDE_LEN * sizeof(float), (void *)(0 * sizeof(float)));
	
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, STRIDE_LEN * sizeof(float), (void *)(3 * sizeof(float)));
	
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, STRIDE_LEN * sizeof(float), (void *)(5 * sizeof(float)));

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, STRIDE_LEN * sizeof(float), (void *)(8 * sizeof(float)));	
}

void					Model::_initTexture(std::string filename)
{
	SDL_Surface *		surface;
	int					colorFormat;

	surface = IMG_Load(ResourceManager::getInstance().getBasePath(filename).c_str());
	if (!surface)
		throw NibblerException("Model::_initTexture() failed on " + filename);
	colorFormat = (surface->format->BytesPerPixel == 4) ? GL_RGBA : GL_RGB;

	glGenTextures(1, &this->_textureID);
	glBindTexture(GL_TEXTURE_2D, this->_textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, colorFormat, surface->w, surface->h,
		0, colorFormat, GL_UNSIGNED_BYTE, surface->pixels);
	SDL_FreeSurface(surface);
}
