#include "Texture.h"
#include "glad.h"
#include <SFML/Graphics.hpp>
#include "stb_image.h"

Texture::Texture()
	: m_currentSlot(0),
	m_ID(0),
	m_width(0),
	m_height(0),
	m_bytesPerPixel(0),
	m_localBuffer(nullptr)
{}

Texture::~Texture()
{
	glDeleteTextures(1, &m_ID);
}

std::unique_ptr<Texture> Texture::loadTexture(const std::string& name)
{
	Texture texture;
	sf::Image image;
	if (!image.loadFromFile("WoW.jpg"))
	{
		return std::unique_ptr<Texture>();
	}


	////stbi_set_flip_vertically_on_load(true);
	//texture.m_localBuffer = stbi_load(name.c_str(), &texture.m_width, &texture.m_height, &texture.m_bytesPerPixel, 4);
	//if (!texture.m_localBuffer)
	//{
	//	
	//}

	glGenTextures(1, &texture.m_ID);
	glBindTexture(GL_TEXTURE_2D, texture.m_ID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.getSize().x, image.getSize().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.getPixelsPtr());
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	//stbi_image_free(texture.m_localBuffer);

	return std::make_unique<Texture>(std::move(texture));
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
	m_currentSlot = slot;
	glActiveTexture(GL_TEXTURE0 + m_currentSlot);
	glBindTexture(GL_TEXTURE_2D, m_ID);
}

void Texture::unbind() const
{
	glBindTexture(GL_TEXTURE_2D, 0);
}