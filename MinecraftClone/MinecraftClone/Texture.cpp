#include "Texture.h"
#include "glad.h"
#include "CubeID.h"
#include <SFML/Graphics.hpp>
#include <iostream>
#include "Utilities.h"
#include <mutex>

//Texture Array
TextureArray::TextureArray(unsigned int slot)
	: m_textureSize(16, 16),
	m_slot(slot),
	m_ID(Utilities::INVALID_OPENGL_ID),
	m_textureCount(0)
{
	glGenTextures(1, &m_ID);
	glActiveTexture(GL_TEXTURE0 + slot);
	bind();
	//glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER,
	//	GL_LINEAR_MIPMAP_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
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

//Texture
//Texture::Texture()
//	: m_currentSlot(0),
//	m_ID(0)
//{}
//
//Texture::~Texture()
//{
//	assert(m_ID != Utilities::INVALID_OPENGL_ID);
//	glDeleteTextures(1, &m_ID);
//}
//
//std::unique_ptr<Texture> Texture::loadTexture(const std::string& name)
//{
//	std::unique_ptr<Texture> texture(new Texture());
//	sf::Image image;
//	if (!image.loadFromFile(name))
//	{
//		return std::unique_ptr<Texture>();
//	}
//	image.flipVertically();
//
//	glGenTextures(1, &texture->m_ID);
//	glBindTexture(GL_TEXTURE_2D_ARRAY, texture->m_ID);
//
//	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//	
//	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGB, 16, 16, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.getSize().x, image.getSize().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.getPixelsPtr());
//	glGenerateMipmap(GL_TEXTURE_2D);
//	glBindTexture(GL_TEXTURE_2D, 0);
//
//	return texture;
//}
//
//unsigned int Texture::getCurrentSlot() const
//{
//	return m_currentSlot;
//}
//
//void Texture::bind(unsigned int slot)
//{
//	m_currentSlot = slot;
//	glActiveTexture(GL_TEXTURE0 + m_currentSlot);
//	glBindTexture(GL_TEXTURE_2D, m_ID);
//}
//
//void Texture::unbind() const
//{
//	glBindTexture(GL_TEXTURE_2D, 0);
//}
