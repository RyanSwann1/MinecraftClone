#include "Gui.h"
#include "glad.h"
#include "Globals.h"
#include "ShaderHandler.h"
#include "Inventory.h"
#include "glm/gtc/matrix_transform.hpp"
#include <assert.h>
#include <array>
#include <iostream>

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
			glm::vec2(0.71f, .92f),
			glm::vec2(0.71f, 1.0f),
			glm::vec2(0.71f, 1.0f),
			glm::vec2(0.0f, 1.0f),
			glm::vec2(0.0f, .92f) };
	};

	std::array<glm::vec2, 6> getSelectionTextCoords()
	{
		return { glm::vec2(0.0f, .828125f),
			glm::vec2(0.1066f, .828125f),
			glm::vec2(0.1066f, 0.9101f),
			glm::vec2(0.1066f, 0.9101f),
			glm::vec2(0.0f, 0.9101f),
			glm::vec2(0.0f, .828125f) };
	};

	eTerrainTextureLayer getTextureLayer(eCubeType cubeType)
	{
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

		return textureLayer;
	}

	glm::vec2 getPositionOnHotbar(eHotbarIndex hotbarIndex, glm::ivec2 basePosition)
	{
		glm::vec2 position = basePosition;
		float offsetX = 75.0f;
		switch (hotbarIndex)
		{
		case eHotbarIndex::One:
			position.x = position.x + offsetX * 1;
			break;
		case eHotbarIndex::Two:
			position.x = position.x + offsetX * 2;
			break;
		case eHotbarIndex::Three:
			position.x = position.x + offsetX * 3;
			break;
		case eHotbarIndex::Four:
			position.x = position.x + offsetX * 4;
			break;
		case eHotbarIndex::Five:
			position.x = position.x + offsetX * 5;
			break;
		case eHotbarIndex::Six:
			position.x = position.x + offsetX * 6;
			break;
		case eHotbarIndex::Seven:
			position.x = position.x + offsetX * 7;
			break;
		case eHotbarIndex::Eight:
			position.x = position.x + offsetX * 8;
			break;
		}

		return position;
	}
}

//Image
Image::Image()
	: m_ID(Globals::INVALID_OPENGL_ID),
	m_positionsVBO(Globals::INVALID_OPENGL_ID),
	m_textCoordsVBO(Globals::INVALID_OPENGL_ID),
	m_active(false)
{	
	glGenVertexArrays(1, &m_ID);
	glBindVertexArray(m_ID);

	glGenBuffers(1, &m_textCoordsVBO);
	glGenBuffers(1, &m_positionsVBO);

	glBindBuffer(GL_ARRAY_BUFFER, m_positionsVBO);
	glBufferData(GL_ARRAY_BUFFER, QUAD_COORDS.size() * sizeof(glm::vec2), QUAD_COORDS.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (const void*)0);

	glBindVertexArray(0);
}

Image::~Image()
{
	if (m_ID != Globals::INVALID_OPENGL_ID)
	{
		glDeleteVertexArrays(1, &m_ID);
	}

	if (m_positionsVBO != Globals::INVALID_OPENGL_ID)
	{
		glDeleteBuffers(1, &m_positionsVBO);
	}

	if (m_textCoordsVBO != Globals::INVALID_OPENGL_ID)
	{
		glDeleteBuffers(1, &m_textCoordsVBO);
	}
}

Image::Image(Image&& orig) noexcept
	: m_ID(orig.m_ID),
	m_positionsVBO(orig.m_positionsVBO),
	m_textCoordsVBO(orig.m_textCoordsVBO),
	m_active(orig.m_active)
{
	orig.m_ID = Globals::INVALID_OPENGL_ID;
	orig.m_positionsVBO = Globals::INVALID_OPENGL_ID;
	orig.m_textCoordsVBO = Globals::INVALID_OPENGL_ID;
	orig.m_active = false;
}

Image& Image::operator=(Image&& orig) noexcept
{
	m_ID = orig.m_ID;
	m_positionsVBO = orig.m_positionsVBO;
	m_textCoordsVBO = orig.m_textCoordsVBO;
	m_active = orig.m_active;

	orig.m_ID = Globals::INVALID_OPENGL_ID;
	orig.m_positionsVBO = Globals::INVALID_OPENGL_ID;
	orig.m_textCoordsVBO = Globals::INVALID_OPENGL_ID;
	orig.m_active = false;

	return *this;
}

bool Image::isActive() const
{
	return m_active;
}

void Image::setActive(bool active)
{
	m_active = active;
}

void Image::setTextureRect(const std::array<glm::vec3, 6>& drawableRect)
{
	glBindVertexArray(m_ID);
	glBindBuffer(GL_ARRAY_BUFFER, m_textCoordsVBO);
	glBufferData(GL_ARRAY_BUFFER, drawableRect.size() * sizeof(glm::vec3), drawableRect.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (const void*)(0));

	glBindVertexArray(0);
}

void Image::render() const
{
	if (m_active)
	{
		glBindVertexArray(m_ID);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
	}
}

//GUI
Gui::Gui()
	: m_items(),
	m_itemID(Globals::INVALID_OPENGL_ID),
	m_itemPositionsVBO(Globals::INVALID_OPENGL_ID),
	m_itemTextCoordsVBO(Globals::INVALID_OPENGL_ID),
	m_toolbarID(Globals::INVALID_OPENGL_ID),
	m_toolbarPostionsVBO(Globals::INVALID_OPENGL_ID),
	m_toolbarTextCoordsVBO(Globals::INVALID_OPENGL_ID),
	m_selectionID(Globals::INVALID_OPENGL_ID),
	m_selectionPositionsVBO(Globals::INVALID_OPENGL_ID),
	m_selectionTextCoordsVBO(Globals::INVALID_OPENGL_ID)
{
	//Initialize Items 
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

	//Initiailize Toolbar
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
	
	//Initialize Selection 
	{
		glGenVertexArrays(1, &m_selectionID);
		glBindVertexArray(m_selectionID);

		glGenBuffers(1, &m_selectionPositionsVBO);
		glBindBuffer(GL_ARRAY_BUFFER, m_selectionPositionsVBO);
		glBufferData(GL_ARRAY_BUFFER, QUAD_COORDS.size() * sizeof(glm::vec2), QUAD_COORDS.data(), GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::ivec2), (const void*)0);

		glGenBuffers(1, &m_selectionTextCoordsVBO);
		glBindBuffer(GL_ARRAY_BUFFER, m_selectionTextCoordsVBO);
		std::array<glm::vec2, 6> textCoords = getSelectionTextCoords();
		glBufferData(GL_ARRAY_BUFFER, textCoords.size() * sizeof(glm::vec2), textCoords.data(), GL_STATIC_DRAW);
		
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (const void*)0);

		glBindVertexArray(0);
	}
}

Gui::~Gui()
{
	//Item
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

	//Toolbar
	assert(m_toolbarID != Globals::INVALID_OPENGL_ID &&
	m_toolbarPostionsVBO != Globals::INVALID_OPENGL_ID &&
	m_toolbarTextCoordsVBO != Globals::INVALID_OPENGL_ID);

	glDeleteVertexArrays(1, &m_toolbarID);
	glDeleteBuffers(1, &m_toolbarPostionsVBO);
	glDeleteBuffers(1, &m_toolbarTextCoordsVBO);

	//Selection
	assert(m_selectionID != Globals::INVALID_OPENGL_ID &&
		m_selectionPositionsVBO != Globals::INVALID_OPENGL_ID &&
		m_selectionTextCoordsVBO != Globals::INVALID_OPENGL_ID);

	glDeleteVertexArrays(1, &m_selectionID);
	glDeleteBuffers(1, &m_selectionPositionsVBO);
	glDeleteBuffers(1, &m_selectionTextCoordsVBO);
}

void Gui::addItem(eHotbarIndex hotbarIndex, eCubeType cubeType)
{
	assert(!m_items[static_cast<int>(hotbarIndex)].isActive());
	if (!m_items[static_cast<int>(hotbarIndex)].isActive())
	{
		m_items[static_cast<int>(hotbarIndex)].setTextureRect(getTextCoords(getTextureLayer(cubeType)));
		m_items[static_cast<int>(hotbarIndex)].setActive(true);
	}
}

void Gui::removeItem(eHotbarIndex hotbarIndex)
{
	assert(m_items[static_cast<int>(hotbarIndex)].isActive());
	m_items[static_cast<int>(hotbarIndex)].setActive(false);
}

void Gui::renderItems(ShaderHandler& shaderHandler) const
{
	for (int i = 0; i < m_items.size(); ++i)
	{
		glm::vec2 position = getPositionOnHotbar(static_cast<eHotbarIndex>(i), { 250, 250 });
		glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(position, 0.0f));
		model = glm::scale(model, glm::vec3(50.0f, 50.0f, 1.0f));

		shaderHandler.switchToShader(eShaderType::UIItem);
		shaderHandler.setUniformMat4f(eShaderType::UIItem, "uModel", model);

		m_items[i].render();
	}
}

void Gui::renderToolbar(ShaderHandler& shaderHandler, glm::vec2 windowSize) const
{
	glBindVertexArray(m_toolbarID);

	glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(glm::vec2(700.0f, 1025.0f), 0.0f));
	model = glm::scale(model, glm::vec3(400.0f, 50.0f, 1.0f));
	shaderHandler.setUniformMat4f(eShaderType::UIToolbar, "uModel", model);

	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void Gui::renderSelectionBox(ShaderHandler& shaderHandler, glm::vec2 windowSize, eHotbarIndex hotBarSelection) const
{
	glm::vec2 position = getPositionOnHotbar(hotBarSelection, { 670, 950 });
	
	glBindVertexArray(m_selectionID);

	glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(glm::vec2(position.x, position.y), 0.0f));
	model = glm::scale(model, glm::vec3(30.0f, 30.0f, 1.0f));
	shaderHandler.setUniformMat4f(eShaderType::UIToolbar, "uModel", model);

	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}
