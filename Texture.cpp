#include "Texture.h"

Texture::Texture()
{
	width = 0;
	height = 0;
	channelsInFile = 0;
	textureID = 0;
	filepath = "";
}

Texture::Texture(const char* filePath)
{
	width = 0;
	height = 0;
	channelsInFile = 0;
	textureID = 0;
	this->filepath = filePath;
}

void Texture::loadTexture()
{
	unsigned char* textureData = stbi_load(filepath, &width, &height, &channelsInFile, 0);

	if (!textureData)
	{
		std::cout << "Unable to load texture!" << std::endl;
	}

	int format = 0;

	if (channelsInFile == 3)
	{
		format = GL_RGB;
	}
	else if (channelsInFile == 4)
	{
		format = GL_RGBA;
	}

	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	stbi_set_flip_vertically_on_load(true);

	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, textureData);
	glGenerateMipmap(GL_TEXTURE_2D);
	//stbi_image_free(textureData);
}

void Texture::useTexture()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);
}

Texture::~Texture()
{
}
