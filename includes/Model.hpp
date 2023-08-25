#ifndef MODEL_HPP
# define MODEL_HPP

# define UNDEFINED_INDEX	-1
# define UNDEFINED_VALUE	-42.0f

# include <string>
# include <fstream>
# include <sstream>
# include <vector>
# include <GL/glew.h>
# include <SDL.h>
# include <SDL_image.h>
# include <glm/glm.hpp>
# include <glm/gtc/matrix_transform.hpp>
# include <glm/gtc/type_ptr.hpp>

typedef struct				s_vec2
{
	float					x;
	float					y;
}							t_vec2;

typedef struct				s_vec3
{
	float					x;
	float					y;
	float					z;
}							t_vec3;

typedef struct				s_f
{
	int						v1;
	int						vt1;
	int						vn1;
	int						v2;
	int						vt2;
	int						vn2;
	int						v3;
	int						vt3;
	int						vn3;
}							t_f;

class Model
{
public:

	Model(std::string objFilename, std::string textureFilename);
	~Model(void);

	const glm::mat4 &		getScaleMatrix(void) const;
	const glm::mat4 &		getRotationMatrix(void) const;
	const glm::mat4 &		getTranslationMatrix(void) const;

	void					setScaleMatrix(glm::mat4 matrix);
	void					setRotationMatrix(glm::mat4 matrix);
	void					setTranslationMatrix(glm::mat4 matrix);

	void					setSpinning(void);
	void					draw(void);

private:

	std::string				_objFilename;
	size_t					_line_number;

	size_t					_triangle_count;

	std::vector<float>		_glData;
	std::vector<t_vec3>		_vData;
	std::vector<t_vec2>		_vtData;
	std::vector<t_vec3>		_vnData;
	std::vector<t_f>		_fData;

	unsigned int			_textureID;

	unsigned int			_vao;
	unsigned int			_vbo;

	glm::mat4				_scaleMatrix;
	glm::mat4				_rotationMatrix;
	glm::mat4				_translationMatrix;
	
	bool					_isSpinning;

	float					_xMin;
	float					_xMax;
	float					_yMin;
	float					_yMax;
	float					_zMin;
	float					_zMax;

	Model(void);
	Model(const Model & src);
	Model & operator=(const Model & rhs);

	void					_readOBJFile(std::string & filename);
	void					_parseOBJLine(std::string & line);
	void					_handleVLine(std::string line);
	void					_handleVTLine(std::string line);
	void					_handleVNLine(std::string line);
	void					_handleFLine(std::string line);
	void					_parseTriangle(std::string expr_1, std::string expr_2, std::string expr_3);
	void					_parseVertexTerm(std::string & expr, std::string & v_str, std::string & vt_str, std::string & vn_str);

	void					_centerAndScaleModel(void);

	void					_generateGLData(void);
	void					_generateGLDataRow(t_vec3 & v, t_vec2 & vt, t_vec3 & vn, glm::vec3 & faceNormal);
	t_vec3					_getV(int vIndex);
	t_vec2					_getVT(int vtIndex);
	t_vec3					_getVN(int vnIndex, glm::vec3 & faceNormal);
	
	void					_initVertexObjects(void);

	void					_initTexture(std::string filename);

	void					_spin(void);

};

#endif
