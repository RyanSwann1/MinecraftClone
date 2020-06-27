#include "Gui.h"
#include "glad.h"
#include "Globals.h"
#include "ShaderHandler.h"
#include "Inventory.h"
#include "glm/gtc/matrix_transform.hpp"
#include "TextureArray.h"
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

	glm::vec2 getPositionOnHotbar(eInventoryIndex hotbarIndex, glm::ivec2 basePosition)
	{
		glm::vec2 position = basePosition;
		float offsetX = 75.0f;
		switch (hotbarIndex)
		{
		case eInventoryIndex::One:
			position.x = position.x + offsetX * 1;
			break;
		case eInventoryIndex::Two:
			position.x = position.x + offsetX * 2;
			break;
		case eInventoryIndex::Three:
			position.x = position.x + offsetX * 3;
			break;
		case eInventoryIndex::Four:
			position.x = position.x + offsetX * 4;
			break;
		case eInventoryIndex::Five:
			position.x = position.x + offsetX * 5;
			break;
		case eInventoryIndex::Six:
			position.x = position.x + offsetX * 6;
			break;
		case eInventoryIndex::Seven:
			position.x = position.x + offsetX * 7;
			break;
		case eInventoryIndex::Eight:
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

void Image::setTextureRect(const std::array<glm::vec2, 6>& drawableRect)
{
	glBindVertexArray(m_ID);
	glBindBuffer(GL_ARRAY_BUFFER, m_textCoordsVBO);
	glBufferData(GL_ARRAY_BUFFER, drawableRect.size() * sizeof(glm::vec2), drawableRect.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (const void*)(0));

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
	m_toolbar(),
	m_selectionBox()
{
	m_toolbar.setTextureRect(getToolbarTextCoords());
	m_toolbar.setActive(true);

	m_selectionBox.setTextureRect(getSelectionTextCoords());
	m_selectionBox.setActive(true);
}

void Gui::addItem(eInventoryIndex hotbarIndex, eCubeType cubeType)
{
	assert(!m_items[static_cast<int>(hotbarIndex)].isActive());
	if (!m_items[static_cast<int>(hotbarIndex)].isActive())
	{
		m_items[static_cast<int>(hotbarIndex)].setTextureRect(getTextCoords(getTextureLayer(cubeType)));
		m_items[static_cast<int>(hotbarIndex)].setActive(true);
	}
}

void Gui::removeItem(eInventoryIndex hotbarIndex)
{
	assert(m_items[static_cast<int>(hotbarIndex)].isActive());
	m_items[static_cast<int>(hotbarIndex)].setActive(false);
}

void Gui::render(ShaderHandler& shaderHandler, const Texture& widgetTexture) const
{
	shaderHandler.switchToShader(eShaderType::UIItem);
	for (int i = 0; i < m_items.size(); ++i)
	{
		glm::vec2 position = getPositionOnHotbar(static_cast<eInventoryIndex>(i), { 250, 250 });
		glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(position, 0.0f));
		model = glm::scale(model, glm::vec3(50.0f, 50.0f, 1.0f));

		shaderHandler.switchToShader(eShaderType::UIItem);
		shaderHandler.setUniformMat4f(eShaderType::UIItem, "uModel", model);

		m_items[i].render();
	}

	widgetTexture.bind();
	shaderHandler.switchToShader(eShaderType::UIToolbar);
	{
		glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(glm::vec2(700.0f, 1025.0f), 0.0f));
		model = glm::scale(model, glm::vec3(400.0f, 50.0f, 1.0f));
		shaderHandler.setUniformMat4f(eShaderType::UIToolbar, "uModel", model);

		m_toolbar.render();
	}

	{
		glm::vec2 position = { 670, 950 };
		glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(glm::vec2(position.x, position.y), 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 30.0f, 1.0f));
		shaderHandler.setUniformMat4f(eShaderType::UIToolbar, "uModel", model);

		m_selectionBox.render();
	}
}