#include "Texture.h"
#include "glad.h"
#include <SFML/Graphics.hpp>
#include "stb_image.h"

Texture::Texture(const std::string& filePath)
	: m_rendererID(0),
	m_filePath(filePath),
	m_localBuffer(nullptr),
	m_width(0),
	m_height(0),
	m_bytesPerPixel(0)
{
	glGenTextures(1, &m_rendererID);
	glBindTexture(GL_TEXTURE_2D, m_rendererID);

	stbi_set_flip_vertically_on_load(1);
	m_localBuffer = stbi_load(filePath.c_str(), &m_width, &m_height, &m_bytesPerPixel, 4);

	glGenTextures(1, &m_rendererID);
	glBindTexture(GL_TEXTURE_2D, m_rendererID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_localBuffer);
	glBindTexture(GL_TEXTURE_2D, 0);

	if (m_localBuffer)
	{
		stbi_image_free(m_localBuffer);
	}
}

Texture::~Texture()
{
	glDeleteTextures(1, &m_rendererID);
}

int Texture::getWidth() const
{
	return 0;
}

int Texture::getHeight() const
{
	return 0;
}

void Texture::bind(unsigned int slot) const
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, m_rendererID);
}

void Texture::unbind() const
{
	glBindTexture(GL_TEXTURE_2D, 0);
}
