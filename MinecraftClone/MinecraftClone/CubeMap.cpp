#include "CubeMap.h"
#include "Utilities.h"
#include "glad.h"
#include <SFML/Graphics.hpp>

namespace
{
	std::array<std::string, 5> SKYBOX_FILEPATHS =
	{
		"skybox_front.jpg",
		"skybox_back.jpg",
		"skybox_left.jpg",
		"skybox_right.jpg",
		"skybox_top.jpg"
	};

	std::array<float, 108> skyboxVertices = {       
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

CubeMap::CubeMap(unsigned int ID)
	: m_ID(ID)
{
	assert(m_ID != Utilities::INVALID_OPENGL_ID);
}

CubeMap::~CubeMap()
{
	assert(m_ID != Utilities::INVALID_OPENGL_ID);
	glDeleteTextures(GL_TEXTURE_CUBE_MAP, &m_ID);
}

std::unique_ptr<CubeMap> CubeMap::create()
{
	unsigned int cubeMapID = Utilities::INVALID_OPENGL_ID;
	glGenTextures(1, &cubeMapID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapID);

	//Load Images
	for (int i = 0; i < SKYBOX_FILEPATHS.size(); ++i)
	{
		sf::Image image;
		if (image.loadFromFile(Utilities::TEXTURE_DIRECTORY + SKYBOX_FILEPATHS[i]))
		{
			image.flipVertically();
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, image.getSize().x, 
				image.getSize().y, 0, GL_RGB, GL_UNSIGNED_BYTE, image.getPixelsPtr());
		}
		else
		{
			glDeleteTextures(1, &cubeMapID);
			return std::unique_ptr<CubeMap>();
		}
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	return std::unique_ptr<CubeMap>(new CubeMap(cubeMapID));
}

void CubeMap::bind() const
{
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_ID);
}

void CubeMap::unbind() const
{
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}
