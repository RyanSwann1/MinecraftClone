#include "Texture.h"
#include "glad.h"
#include <SFML/Graphics.hpp>
#include <iostream>

Texture::Texture()
	: m_currentSlot(0),
	m_ID(0)
{}

Texture::~Texture()
{
	glDeleteTextures(1, &m_ID);
}

std::unique_ptr<Texture> Texture::loadTexture(const std::string& name)
{
	std::unique_ptr<Texture> texture(new Texture());
	sf::Image image;
	if (!image.loadFromFile(name))
	{
		return std::unique_ptr<Texture>();
	}

	image.flipVertically();

	glGenTextures(1, &texture->m_ID);
	glBindTexture(GL_TEXTURE_2D, texture->m_ID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.getSize().x, image.getSize().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.getPixelsPtr());
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	return std::unique_ptr<Texture>(std::move(texture));
}

unsigned int Texture::getCurrentSlot() const
{
	return m_currentSlot;
}

unsigned int Texture::getID() const
{
	return m_ID;
}

void Texture::getTextCoords(eTileID tileID, std::vector<float>& textCoords) const
{
	switch (tileID)
	{
	case eTileID::Grass :
		textCoords.push_back(0.0);
		textCoords.push_back((128.0f - 16.0f) / 128.0f);

		textCoords.push_back(16.0f / 128.0f);
		textCoords.push_back((128.0f - 16.0f) / 128.0f);

		textCoords.push_back(16.0f / 128.0f);
		textCoords.push_back(1);
		
		textCoords.push_back(0);
		textCoords.push_back(1);
	
		break;
	case eTileID::DirtSide :
		textCoords.push_back(16.0f / 128.0f);
		textCoords.push_back((128.0f - 16.0f) / 128.0f);

		textCoords.push_back(32.0f / 128.0f);
		textCoords.push_back((128.0f - 16.0f) / 128.0f);
		
		textCoords.push_back(32.0f / 128.0f);
		textCoords.push_back(1);
		
		textCoords.push_back(16.0f / 128.0f);
		textCoords.push_back(1);

		break;
	case eTileID::Dirt :
		textCoords.push_back(32.0f / 128.0f);
		textCoords.push_back((128.0f - 16.0f) / 128.0f);

		textCoords.push_back(48.0f / 128.0f);
		textCoords.push_back((128.0f - 16.0f) / 128.0f);

		textCoords.push_back(48.0f / 128.0f);
		textCoords.push_back(1);

		textCoords.push_back(32.0f / 128.0f);
		textCoords.push_back(1);
		
		break;
	}
}

void Texture::bind(unsigned int slot)
{
	m_currentSlot = slot;
	glActiveTexture(GL_TEXTURE0 + m_currentSlot);
	glBindTexture(GL_TEXTURE_2D, m_ID);
}

void Texture::unbind() const
{
	glBindTexture(GL_TEXTURE_2D, 0);
}