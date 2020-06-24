#include "Gui.h"
#include "glad.h"
#include "Globals.h"
#include <assert.h>
#include <array>

namespace
{
	constexpr std::array<glm::ivec2, 6> QUAD_COORDS =
	{
		glm::ivec2(0, 0),
		glm::ivec2(250, 0),
		glm::ivec2(250, 250),
		glm::ivec2(250, 250),
		glm::ivec2(0, 250),
		glm::ivec2(0, 0)
	};

	std::array<glm::ivec3, 6> getTextCoords(eTextureLayer textureLayer) 
	{
		return { glm::ivec3(0, 0, static_cast<int>(textureLayer)),
			glm::ivec3(1, 0, static_cast<int>(textureLayer)),
			glm::ivec3(1, 1, static_cast<int>(textureLayer)),
			glm::ivec3(1, 1, static_cast<int>(textureLayer)),
			glm::ivec3(0, 1, static_cast<int>(textureLayer)),
			glm::ivec3(0, 0, static_cast<int>(textureLayer)) };
	};
}

Gui::Gui()
	: m_ID(Globals::INVALID_OPENGL_ID)
{
	glGenVertexArrays(1, &m_ID);
	glBindVertexArray(m_ID);
	
	{
		//Positions VBO
		unsigned int positionsVBO;
		glGenBuffers(1, &positionsVBO);

		glBindBuffer(GL_ARRAY_BUFFER, positionsVBO);
		glBufferData(GL_ARRAY_BUFFER, QUAD_COORDS.size() * sizeof(glm::ivec2), QUAD_COORDS.data(), GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_INT, GL_FALSE, sizeof(glm::ivec2), (const void*)0);

		glDeleteBuffers(1, &positionsVBO);
	}

	{
		//TextCoords VBO
		unsigned int textCoordsVBO;
		glGenBuffers(1, &textCoordsVBO);

		std::array<glm::ivec3, 6> textCoords = getTextCoords(eTextureLayer::Dirt);

		glBindBuffer(GL_ARRAY_BUFFER, textCoordsVBO);
		glBufferData(GL_ARRAY_BUFFER, textCoords.size() * sizeof(glm::ivec3), textCoords.data(), GL_STATIC_DRAW);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_INT, GL_FALSE, sizeof(glm::ivec3), (const void*)(0));

		glDeleteBuffers(1, &textCoordsVBO);
	}
	
	glBindVertexArray(0);
}

Gui::~Gui()
{
	assert(m_ID != Globals::INVALID_OPENGL_ID);
	glDeleteVertexArrays(1, &m_ID);
}

void Gui::render() const
{
	glBindVertexArray(m_ID);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}