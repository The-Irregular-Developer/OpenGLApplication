#pragma once

#include <GL/glew.h>
#include "stb_image.h"
#include <iostream>


class Texture
{
public:
	Texture();
	Texture(const char* filePath);
	void loadTexture();
	void useTexture();
	~Texture();
private:
	GLint width, height, channelsInFile;
	const char* filepath;
	GLuint textureID;
};

