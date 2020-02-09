#include "Texture.h"
#include "glad.h"
#include <SFML/Graphics.hpp>
#include "stb_image.h"
#include <iostream>

Texture::Texture()
	: m_currentSlot(0),
	m_ID(0)
{}

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

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.getSize().x, image.getSize().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.getPixelsPtr());
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	return texture;
}

Texture::~Texture()
{
	glDeleteTextures(1, &m_ID);
}

unsigned int Texture::getCurrentSlot() const
{
	return m_currentSlot;
}

unsigned int Texture::getID() const
{
	return m_ID;
}

void Texture::bind(unsigned int slot)
{
	std::cout << glGetError() << "No Error\n";
	m_currentSlot = slot;
	glActiveTexture(GL_TEXTURE0 + m_currentSlot);
	glBindTexture(GL_TEXTURE_2D, m_ID);
	std::cout << glGetError() << "\n";
}

void Texture::unbind() const
{
	glBindTexture(GL_TEXTURE_2D, 0);
}