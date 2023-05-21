#ifndef TEXTURE_H
#define TEXTURE_H

#define GLEW_STATIC
#include "GL/glew.h"
#include <string>

using namespace std;

class Texture
{
public:
	Texture();
	~Texture();

	bool loadTexture(const string& fileName, bool generateMipMap = true);
	void bind(GLuint texUnit = 0);
	void unbind(GLuint texUnit);

private:
	GLuint mTexture;
};

#endif
