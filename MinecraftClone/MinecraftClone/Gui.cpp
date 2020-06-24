#include "Gui.h"
#include "glad.h"
#include "Globals.h"
#include "ShaderHandler.h"
#include "glm/gtc/matrix_transform.hpp"
#include <assert.h>
#include <array>

namespace
{
	constexpr std::array<glm::ivec2, 6> QUAD_COORDS =
	{
		glm::ivec2(0, 0),
		glm::ivec2(1, 0),
		glm::ivec2(1, 1),
		glm::ivec2(1, 1),
		glm::ivec2(0, 1),
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
	: m_ID(Globals::INVALID_OPENGL_ID),
	m_positionsVBO(Globals::INVALID_OPENGL_ID),
	m_textCoordsVBO(Globals::INVALID_OPENGL_ID)
{
	glGenVertexArrays(1, &m_ID);
	glBindVertexArray(m_ID);
		
	glGenBuffers(1, &m_textCoordsVBO);
	glGenBuffers(1, &m_positionsVBO);

	glBindBuffer(GL_ARRAY_BUFFER, m_positionsVBO);
	glBufferData(GL_ARRAY_BUFFER, QUAD_COORDS.size() * sizeof(glm::ivec2), QUAD_COORDS.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_INT, GL_FALSE, sizeof(glm::ivec2), (const void*)0);

	
	glBindVertexArray(0);
}

Gui::~Gui()
{
	assert(m_ID != Globals::INVALID_OPENGL_ID);
	glDeleteVertexArrays(1, &m_ID);

	if (m_positionsVBO != Globals::INVALID_OPENGL_ID)
	{
		glDeleteBuffers(1, &m_positionsVBO);
	}

	if (m_textCoordsVBO != Globals::INVALID_OPENGL_ID)
	{
		glDeleteBuffers(1, &m_textCoordsVBO);
	}
}

void Gui::drawSprite(eCubeType cubeType, ShaderHandler& shaderHandler, glm::vec2 windowSize) const
{
	shaderHandler.switchToShader(eShaderType::UI);

	glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(windowSize.x), static_cast<float>(windowSize.y), 0.0f, -1.0f, 1.0f);
	shaderHandler.setUniformMat4f(eShaderType::UI, "uProjection", projection);
	
	eTextureLayer textureLayer = eTextureLayer::Error;
	switch (cubeType)
	{
	case eCubeType::Grass:
		textureLayer = eTextureLayer::GrassSide;
		break;
	case eCubeType::Cactus:
	case eCubeType::CactusTop:
		textureLayer = eTextureLayer::Cactus;
		break;
	case eCubeType::Dirt:
		textureLayer = eTextureLayer::Dirt;
		break;
	case eCubeType::Sand:
		textureLayer = eTextureLayer::Sand;
		break;
	case eCubeType::Shrub:
		textureLayer = eTextureLayer::Shrub;
		break;
	case eCubeType::TallGrass:
		textureLayer = eTextureLayer::TallGrass;
		break;
	case eCubeType::Leaves:
		textureLayer = eTextureLayer::Leaves;
		break;
	case eCubeType::Log:
	case eCubeType::LogTop:
		textureLayer = eTextureLayer::Log;
		break;
	case eCubeType::Stone:
		textureLayer = eTextureLayer::Stone;
		break;
	default:
		assert(false);
	}

	std::array<glm::ivec3, 6> textCoords = getTextCoords(textureLayer);

	glBindVertexArray(m_ID);
	glBindBuffer(GL_ARRAY_BUFFER, m_textCoordsVBO);
	glBufferData(GL_ARRAY_BUFFER, textCoords.size() * sizeof(glm::ivec3), textCoords.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_INT, GL_FALSE, sizeof(glm::ivec3), (const void*)(0));

	glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(glm::vec2(50.0f, 50.0f), 0.0f));
	model = glm::scale(model, glm::vec3(50.0f, 50.0f, 1.0f));
	shaderHandler.setUniformMat4f(eShaderType::UI, "uModel", model);

	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}