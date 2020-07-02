#include "TextureArray.h"
#include "glad.h"
#include "CubeType.h"
#include <SFML/Graphics.hpp>
#include <iostream>
#include "Globals.h"
#include <array>

namespace
{
	constexpr glm::ivec2 TEXTURE_SIZE = { 16, 16 };

	const std::array<std::string, 14> TERRAIN_TEXTURE_FILENAMES =
	{
		"grass.png",
		"grass_side.png",
		"dirt.png",
		"sand.png",
		"stone.png",
		"water.png",
		"log.png",
		"logtop.png",
		"leaves.png",
		"common_cactus_side.png",
		"common_cactus_top.png",
		"common_dead_shrub.png",
		"common_tall_grass.png",
		"error.png"
	};

	bool addTexture(const std::string& textureName)
	{
		sf::Image image;
		bool textureLoaded = image.loadFromFile(Globals::TEXTURE_DIRECTORY + textureName);
		assert(textureLoaded);
		if (!textureLoaded)
		{
			std::cout << textureName << " not loaded.\n";
			return false;
		}
		image.flipVertically();
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.getSize().x, image.getSize().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.getPixelsPtr());
		glGenerateMipmap(GL_TEXTURE_2D);

		return true;
	}

	bool addTexture(const std::string& textureName, int textureCountIndex)
	{
		sf::Image image;
		bool textureLoaded = image.loadFromFile(Globals::TEXTURE_DIRECTORY + textureName);
		assert(textureLoaded);
		if (!textureLoaded)
		{
			std::cout << textureName << " not loaded.\n";
			return false;
		}
		image.flipVertically();
		glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, textureCountIndex, TEXTURE_SIZE.x, TEXTURE_SIZE.y,
			1, GL_RGBA, GL_UNSIGNED_BYTE, image.getPixelsPtr());

		glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER,
			GL_LINEAR_MIPMAP_LINEAR);
		glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_LOD_BIAS, -1);

		return true;
	}
}

//Texture
Texture::Texture(unsigned int ID)
	: m_ID(ID)
{

}

Texture::~Texture()
{
	assert(m_ID != Globals::INVALID_OPENGL_ID);
	glDeleteTextures(1, &m_ID);
}

void Texture::bind() const
{
	glBindTexture(GL_TEXTURE_2D, m_ID);
}

void Texture::unbind() const
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

std::unique_ptr<Texture> Texture::create(const std::string& textureName)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	glBindTexture(GL_TEXTURE_2D, textureID);

	if (!addTexture(textureName))
	{
		return std::unique_ptr<Texture>();
	}
	else
	{
		return std::unique_ptr<Texture>(new Texture(textureID));
	}
}

//Texture Array
TextureArray::TextureArray(unsigned int ID)
	: m_slot(0),
	m_ID(ID)
{
	//glActiveTexture(GL_TEXTURE0 + m_slot);
}

std::unique_ptr<TextureArray> TextureArray::create()
{
	unsigned int textureArrayID = Globals::INVALID_OPENGL_ID;
	glGenTextures(1, &textureArrayID);

	glBindTexture(GL_TEXTURE_2D_ARRAY, textureArrayID);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER,
		GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA, TEXTURE_SIZE.x, TEXTURE_SIZE.y, static_cast<int>(eTerrainTextureLayer::Max) + 1, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

	for (int i = 0; i < TERRAIN_TEXTURE_FILENAMES.size(); ++i)
	{
		bool textureAdded = addTexture(TERRAIN_TEXTURE_FILENAMES[i], i);
		assert(textureAdded);
		if (!textureAdded)
		{
			glDeleteTextures(1, &textureArrayID);
			return std::unique_ptr<TextureArray>();
		}
	}

	return std::unique_ptr<TextureArray>(new TextureArray(textureArrayID));
}

TextureArray::~TextureArray()
{
	assert(m_ID != Globals::INVALID_OPENGL_ID);
	glDeleteTextures(1, &m_ID);
}

unsigned int TextureArray::getCurrentSlot() const
{
	return m_slot;
}

void TextureArray::bind() const
{
	//glActiveTexture(GL_TEXTURE0 + m_ID);
	//glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, m_ID);
}

void TextureArray::unbind() const
{
	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}