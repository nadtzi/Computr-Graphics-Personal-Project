#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION // becasue it is a header file type library
#include "std_image/stb_image.h"


Texture::Texture() :mTexture(0) {

}

Texture::~Texture() {
	glDeleteTextures(1, &mTexture);
}

bool Texture::loadTexture(const string& fileName, bool generateMipMaps) 
{
	
	int width, height, bitdepth, nrcomponent;
	glGenTextures(1, &mTexture);
	glBindTexture(GL_TEXTURE_2D, mTexture);

	//set texture parameters filters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//set texture parameters wrap
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//load and create texture
	unsigned char* texelData = stbi_load(fileName.c_str(), &width, &height, &nrcomponent, 0);
	if (!texelData)
	{
		printf("Failing to load iamge file %s", fileName.c_str());
	}

	GLenum format = 0;
	if (nrcomponent == 0)
		format = GL_RED;
	if (nrcomponent == 3)
		format = GL_RGB;
	if (nrcomponent == 4)
		format = GL_RGBA;


	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, texelData);
	glGenerateMipmap(GL_TEXTURE_2D);

	if (generateMipMaps)
		glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0); //unbinding

	stbi_image_free(texelData);

	return true;
}

void Texture::bind(GLuint texUnit) {
	glActiveTexture(GL_TEXTURE0 + texUnit);
	glBindTexture(GL_TEXTURE_2D, mTexture);
}

void Texture::unbind(GLuint texUnit) {
	glActiveTexture(GL_TEXTURE0 + texUnit);
	glBindTexture(GL_TEXTURE_2D, 0); //unbinding 
}