#include "Gui.h"
#include "glad.h"
#include "Globals.h"
#include "ShaderHandler.h"
#include "glm/gtc/matrix_transform.hpp"
#include <assert.h>
#include <array>

namespace
{
	constexpr std::array<glm::vec2, 6> QUAD_COORDS =
	{
		glm::vec2(0, 0),
		glm::vec2(1, 0),
		glm::vec2(1, 1),
		glm::vec2(1, 1),
		glm::vec2(0, 1),
		glm::vec2(0, 0)
	};

	std::array<glm::vec3, 6> getTextCoords(eTerrainTextureLayer textureLayer) 
	{
		return { glm::vec3(0, 0, static_cast<int>(textureLayer)),
			glm::vec3(1, 0, static_cast<int>(textureLayer)),
			glm::vec3(1, 1, static_cast<int>(textureLayer)),
			glm::vec3(1, 1, static_cast<int>(textureLayer)),
			glm::vec3(0, 1, static_cast<int>(textureLayer)),
			glm::vec3(0, 0, static_cast<int>(textureLayer)) };
	};

	std::array<glm::vec2, 6> getToolbarTextCoords()
	{
		return { glm::vec2(0.0f, .92f),
			glm::vec2(0.7f, .92f),
			glm::vec2(0.7f, 1.0f),
			glm::vec2(0.7f, 1.0f),
			glm::vec2(0.0f, 1.0f),
			glm::vec2(0.0f, .92f) };
	};
}

Gui::Gui()
	: m_itemID(Globals::INVALID_OPENGL_ID),
	m_itemPositionsVBO(Globals::INVALID_OPENGL_ID),
	m_itemTextCoordsVBO(Globals::INVALID_OPENGL_ID),
	m_toolbarID(Globals::INVALID_OPENGL_ID),
	m_toolbarPostionsVBO(Globals::INVALID_OPENGL_ID),
	m_toolbarTextCoordsVBO(Globals::INVALID_OPENGL_ID)
{
	//Initialize Items GUI
	{
		glGenVertexArrays(1, &m_itemID);
		glBindVertexArray(m_itemID);

		glGenBuffers(1, &m_itemTextCoordsVBO);
		glGenBuffers(1, &m_itemPositionsVBO);

		glBindBuffer(GL_ARRAY_BUFFER, m_itemPositionsVBO);
		glBufferData(GL_ARRAY_BUFFER, QUAD_COORDS.size() * sizeof(glm::vec2), QUAD_COORDS.data(), GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (const void*)0);

		glBindVertexArray(0);
	}

	//Initiailize Toolbar GUI
	{
		glGenVertexArrays(1, &m_toolbarID);
		glBindVertexArray(m_toolbarID);

		glGenBuffers(1, &m_toolbarPostionsVBO);
		glBindBuffer(GL_ARRAY_BUFFER, m_toolbarPostionsVBO);
		glBufferData(GL_ARRAY_BUFFER, QUAD_COORDS.size() * sizeof(glm::vec2), QUAD_COORDS.data(), GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::ivec2), (const void*)0);

		glGenBuffers(1, &m_toolbarTextCoordsVBO);
		glBindBuffer(GL_ARRAY_BUFFER, m_toolbarTextCoordsVBO);
		std::array<glm::vec2, 6> toolbarTextCoords = getToolbarTextCoords();
		glBufferData(GL_ARRAY_BUFFER, toolbarTextCoords.size() * sizeof(glm::vec2), toolbarTextCoords.data(), GL_STATIC_DRAW);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (const void*)0);

		glBindVertexArray(0);
	}
}

Gui::~Gui()
{
	assert(m_itemID != Globals::INVALID_OPENGL_ID);
	glDeleteVertexArrays(1, &m_itemID);

	if (m_itemPositionsVBO != Globals::INVALID_OPENGL_ID)
	{
		glDeleteBuffers(1, &m_itemPositionsVBO);
	}

	if (m_itemTextCoordsVBO != Globals::INVALID_OPENGL_ID)
	{
		glDeleteBuffers(1, &m_itemTextCoordsVBO);
	}

	assert(m_toolbarID != Globals::INVALID_OPENGL_ID);
	glDeleteVertexArrays(1, &m_toolbarID);

	if (m_toolbarPostionsVBO != Globals::INVALID_OPENGL_ID)
	{
		glDeleteBuffers(1, &m_toolbarPostionsVBO);
	}

	if (m_toolbarTextCoordsVBO != Globals::INVALID_OPENGL_ID)
	{
		glDeleteBuffers(1, &m_toolbarTextCoordsVBO);
	}
}

void Gui::renderSprite(eCubeType cubeType, ShaderHandler& shaderHandler, glm::vec2 windowSize) const
{
	shaderHandler.switchToShader(eShaderType::UIItem);

	glm::mat4 projection = glm::ortho(0.0f, windowSize.x, windowSize.y, 0.0f, -1.0f, 1.0f);
	shaderHandler.setUniformMat4f(eShaderType::UIItem, "uProjection", projection);
	
	eTerrainTextureLayer textureLayer = eTerrainTextureLayer::Error;
	switch (cubeType)
	{
	case eCubeType::Grass:
		textureLayer = eTerrainTextureLayer::GrassSide;
		break;
	case eCubeType::Cactus:
	case eCubeType::CactusTop:
		textureLayer = eTerrainTextureLayer::Cactus;
		break;
	case eCubeType::Dirt:
		textureLayer = eTerrainTextureLayer::Dirt;
		break;
	case eCubeType::Sand:
		textureLayer = eTerrainTextureLayer::Sand;
		break;
	case eCubeType::Log:
	case eCubeType::LogTop:
		textureLayer = eTerrainTextureLayer::Log;
		break;
	case eCubeType::Stone:
		textureLayer = eTerrainTextureLayer::Stone;
		break;
	default:
		assert(false);
	}

	glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(glm::vec2(50.0f, 50.0f), 0.0f));
	model = glm::scale(model, glm::vec3(50.0f, 50.0f, 1.0f));
	shaderHandler.setUniformMat4f(eShaderType::UIItem, "uModel", model);

	std::array<glm::vec3, 6> textCoords = getTextCoords(textureLayer);
	glBindVertexArray(m_itemID);
	glBindBuffer(GL_ARRAY_BUFFER, m_itemTextCoordsVBO);
	glBufferData(GL_ARRAY_BUFFER, textCoords.size() * sizeof(glm::vec3), textCoords.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (const void*)(0));

	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void Gui::renderToolbar(ShaderHandler& shaderHandler, glm::vec2 windowSize) const
{
	shaderHandler.switchToShader(eShaderType::UIToolbar);
	glBindVertexArray(m_toolbarID);

	glm::mat4 projection = glm::ortho(0.0f, windowSize.x, windowSize.y, 0.0f, -1.0f, 1.0f);
	shaderHandler.setUniformMat4f(eShaderType::UIToolbar, "uProjection", projection);

	glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(glm::vec2(250.0f, 250.0f), 0.0f));
	model = glm::scale(model, glm::vec3(400.0f, 50.0f, 1.0f));
	shaderHandler.setUniformMat4f(eShaderType::UIToolbar, "uModel", model);

	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}