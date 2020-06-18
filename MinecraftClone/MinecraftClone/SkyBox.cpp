#include "SkyBox.h"
#include "Globals.h"
#include "glad.h"
#include <SFML/Graphics.hpp>
#include <array>

namespace
{
	const std::array<std::string, 6> SKYBOX_FILEPATHS =
	{
		"DaylightBox_Right.bmp",
		"DaylightBox_Left.bmp",
		"DaylightBox_Top.bmp",
		"DaylightBox_Bottom.bmp",
		"DaylightBox_Front.bmp",
		"DaylightBox_Back.bmp"
	};

	constexpr std::array<float, 108> SKYBOX_VERTICES = 
	{       
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};
}

SkyBox::SkyBox(unsigned int cubeMapID)
	: m_cubeMapID(cubeMapID),
	m_vaoID(Globals::INVALID_OPENGL_ID),
	m_vboID(Globals::INVALID_OPENGL_ID)

{
	assert(m_cubeMapID != Globals::INVALID_OPENGL_ID);

	glGenBuffers(1, &m_vboID);
	glGenVertexArrays(1, &m_vaoID);
	glBindVertexArray(m_vaoID);

	glBindBuffer(GL_ARRAY_BUFFER, m_vboID);
	glBufferData(GL_ARRAY_BUFFER, SKYBOX_VERTICES.size() * sizeof(float), SKYBOX_VERTICES.data(), GL_STATIC_DRAW);
	

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (const void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

SkyBox::~SkyBox()
{
	assert(m_cubeMapID != Globals::INVALID_OPENGL_ID);
	glDeleteTextures(GL_TEXTURE_CUBE_MAP, &m_cubeMapID);

	assert(m_vaoID != Globals::INVALID_OPENGL_ID);
	glDeleteVertexArrays(1, &m_vaoID);

	assert(m_vboID != Globals::INVALID_OPENGL_ID);
	glDeleteBuffers(1, &m_vboID);
}

std::unique_ptr<SkyBox> SkyBox::create()
{
	unsigned int cubeMapID = Globals::INVALID_OPENGL_ID;
	glGenTextures(1, &cubeMapID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapID);

	//Load Images
	for (int i = 0; i < SKYBOX_FILEPATHS.size(); ++i)
	{
		sf::Image image;
		if (image.loadFromFile(Globals::TEXTURE_DIRECTORY + SKYBOX_FILEPATHS[i]))
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, image.getSize().x, 
				image.getSize().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.getPixelsPtr());
		}
		else
		{
			glDeleteTextures(1, &cubeMapID);
			return std::unique_ptr<SkyBox>();
		}
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	return std::unique_ptr<SkyBox>(new SkyBox(cubeMapID));
}

void SkyBox::render() const
{
	glBindVertexArray(m_vaoID);

	glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubeMapID);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}