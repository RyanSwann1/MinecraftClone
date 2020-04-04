#include "Texture.h"
#include "glad.h"
#include "CubeID.h"
#include <SFML/Graphics.hpp>
#include <iostream>
#include "Utilities.h"

TextureArray::TextureArray(unsigned int slot)
	: m_textureSize(16, 16),
	m_slot(slot),
	m_ID(Utilities::INVALID_OPENGL_ID),
	m_textureCount(0)
{
	glGenTextures(1, &m_ID);
	glActiveTexture(GL_TEXTURE0 + slot);
	bind();
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA, m_textureSize.x, m_textureSize.y, static_cast<int>(eTextureLayer::Max) + 1, 0, 
		GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
}

unsigned int TextureArray::getCurrentSlot() const
{
	return m_slot;
}

bool TextureArray::addTexture(const std::string& textureName)
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
	glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, m_textureCount, m_textureSize.x, m_textureSize.y, 
		1, GL_RGBA, GL_UNSIGNED_BYTE, image.getPixelsPtr());
	++m_textureCount;

	glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return true;
}

void TextureArray::bind() const
{
	glBindTexture(GL_TEXTURE_2D_ARRAY, m_ID);
}