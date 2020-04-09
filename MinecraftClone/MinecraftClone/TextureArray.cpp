#include "TextureArray.h"
#include "glad.h"
#include "CubeID.h"
#include <SFML/Graphics.hpp>
#include <iostream>
#include "Utilities.h"

namespace
{
	constexpr glm::ivec2 TEXTURE_SIZE = { 16, 16 };

	const std::array<std::string, 12> TEXTURE_FILENAMES =
	{
		"grass.png",
		"grass_side.png",
		"sand.png",
		"stone.png",
		"water.png",
		"log.png",
		"logtop.png",
		"leaves.png",
		"common_cactus_side.png",
		"common_dead_shrub.png",
		"common_tall_grass.png",
		"error.png"
	};

	bool addTexture(const std::string& textureName, int textureCountIndex)
	{
		sf::Image image;
		bool textureLoaded = image.loadFromFile(Utilities::TEXTURE_DIRECTORY + textureName);
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

TextureArray::TextureArray(unsigned int ID)
	: m_slot(0),
	m_ID(ID)
{
	assert(ID != Utilities::INVALID_OPENGL_ID);
	glActiveTexture(GL_TEXTURE0 + m_slot);
}

std::unique_ptr<TextureArray> TextureArray::create()
{
	unsigned int textureArrayID = Utilities::INVALID_OPENGL_ID;
	glGenTextures(1, &textureArrayID);

	glBindTexture(GL_TEXTURE_2D_ARRAY, textureArrayID);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER,
		GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA, TEXTURE_SIZE.x, TEXTURE_SIZE.y, static_cast<int>(eTextureLayer::Max) + 1, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

	for (int i = 0; i < TEXTURE_FILENAMES.size(); ++i)
	{
		bool textureAdded = addTexture(TEXTURE_FILENAMES[i], i);
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
	assert(m_ID != Utilities::INVALID_OPENGL_ID);
	glDeleteTextures(1, &m_ID);
}

unsigned int TextureArray::getCurrentSlot() const
{
	return m_slot;
}

void TextureArray::bind() const
{
	glBindTexture(GL_TEXTURE_2D_ARRAY, m_ID);
}

void TextureArray::unbind() const
{
	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}
