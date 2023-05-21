//--------------------
/// <summary>
///  GLSL shader program
/// </summary>


#define GLEW_STATIC
#include "GL/glew.h"
#include <string>

#include "glm/glm.hpp"
#include <map>

#ifndef SHADER_H
#define SHADER_H

using std::string;

class ShaderProgram
{
public:
	ShaderProgram();
	~ShaderProgram();

	enum ShaderType {
		VERTEX,
		FRAGMENT,
		PROGRAM
	};

	bool loadShaders(const char* vsfilename, const char* fsfilename);// read shaders from file
	void use();
	GLuint getProgram() const;

	void setUniform(const GLchar* name, const glm::vec2& v);
	void setUniform(const GLchar* name, const glm::vec3& v);
	void setUniform(const GLchar* name, const glm::vec4& v);
	void setUniform(const GLchar* name, const glm::mat4& m);

private:
	GLuint mHandle;

	string fileToString(const string& filename);
	void checkCompileErrors(GLuint shader, ShaderType type);

	GLint getUniformLocation(const GLchar* name);
	std::map<string, GLint>  mUniformLocations;


};

#endif #endif // SHADER_H