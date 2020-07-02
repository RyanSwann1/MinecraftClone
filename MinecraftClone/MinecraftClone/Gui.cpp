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
	constexpr size_t MAX_DIGITS = 2;
	constexpr int TEXT_SIZE = 52;
	constexpr int CHARACTER_SPACING = TEXT_SIZE / 3;
	constexpr int FONT_TEXTURE_COLUMNS = 8;
	constexpr glm::vec2 FONT_TEXTURE_TILESIZE = { 32, 32 };
	constexpr glm::vec2 FONT_TEXTURE_SIZE = { 256, 256 };
	constexpr glm::vec2 ITEM_SIZE = { 36, 36 };

	constexpr glm::vec2 TOOLBAR_STARTING_POSITION = { 700.0f, 1000.0f };
	constexpr glm::vec2 INITIAL_ITEM_STARTING_POSITION = { 710.0f, 1000.0f };
	constexpr glm::vec2 ITEM_QUANTITY_STARTING_POSITION = { 720.0f, 975.0f };

	std::array<glm::vec2, 6> getQuadCoords(const glm::vec2& position, const glm::vec2& scale)
	{
		return {
			position, 
			glm::vec2(position.x + scale.x, position.y),
			glm::vec2(position.x + scale.x, position.y + scale.y),
			glm::vec2(position.x + scale.x, position.y + scale.y),
			glm::vec2(position.x, position.y + scale.y),
			glm::vec2(position.x, position.y)
		};
	};

	char convertDigit(int i)
	{
		return static_cast<char>('0' + i);
	}

	int convertCharacter(char c)
	{
		return static_cast<int>(c - '0');
	}

	glm::vec2 convertTo2DTextCoord(int ID, glm::vec2 tileSize, int columns, glm::vec2 textureSize)
	{
		glm::vec2 position((ID % columns) / static_cast<float>(columns), (ID / columns) / static_cast<float>(columns));
		position.y = 1.0f - position.y;

		return position;
	}

	std::array<glm::vec2, 6> getCharacterTextCoords(const glm::vec2& startingPosition, int columns)
	{
		float x = 1.0f / static_cast<float>(columns);
		float y = 1.0f / static_cast<float>(columns);

		return {
			glm::vec2(startingPosition.x, startingPosition.y - y),
			glm::vec2(startingPosition.x + x, startingPosition.y - y),
			glm::vec2(startingPosition.x + x, startingPosition.y),
			glm::vec2(startingPosition.x + x, startingPosition.y),
			glm::vec2(startingPosition.x, startingPosition.y),
			glm::vec2(startingPosition.x, startingPosition.y - y)
		};
	}

	std::array<glm::vec3, 6> getTextCoords(eTerrainTextureLayer textureLayer) 
	{
		return { glm::vec3(0, 0, static_cast<int>(textureLayer)),
			glm::vec3(1, 0, static_cast<int>(textureLayer)),
			glm::vec3(1, 1, static_cast<int>(textureLayer)),
			glm::vec3(1, 1, static_cast<int>(textureLayer)),
			glm::vec3(0, 1, static_cast<int>(textureLayer)),
			glm::vec3(0, 0, static_cast<int>(textureLayer)) };
	};

	constexpr std::array<glm::vec2, 6> toolbarTextCoords =
	{
		glm::vec2(0.0f, .92f),
		glm::vec2(0.71f, .92f),
		glm::vec2(0.71f, 1.0f),
		glm::vec2(0.71f, 1.0f),
		glm::vec2(0.0f, 1.0f),
		glm::vec2(0.0f, .92f)
	};

	constexpr std::array<glm::vec2, 6> selectionBoxTextCoords =
	{
		glm::vec2(0.0f, .828125f),
		glm::vec2(0.09f, .828125f),
		glm::vec2(0.09f, 0.9101f),
		glm::vec2(0.09f, 0.9101f),
		glm::vec2(0.0f, 0.9101f),
		glm::vec2(0.0f, .828125f)
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
		float offsetX = 55.0f;
		switch (hotbarIndex)
		{
		case eInventoryIndex::One:
			position.x = position.x;
			break;
		case eInventoryIndex::Two:
			position.x = position.x + offsetX * 1;
			break;
		case eInventoryIndex::Three:
			position.x = position.x + offsetX * 2;
			break;
		case eInventoryIndex::Four:
			position.x = position.x + offsetX * 3;
			break;
		case eInventoryIndex::Five:
			position.x = position.x + offsetX * 4;
			break;
		case eInventoryIndex::Six:
			position.x = position.x + offsetX * 5;
			break;
		case eInventoryIndex::Seven:
			position.x = position.x + offsetX * 6;
			break;
		case eInventoryIndex::Eight:
			position.x = position.x + offsetX * 7;
			break;
		}

		return position;
	}

	void collectDigits(std::array<int, MAX_DIGITS>& digits, int quantity, int& i)
	{
		if (quantity >= 10)
		{
			collectDigits(digits, quantity / 10, i);
		}

		digits[i] = quantity % 10;
		i++;
		assert(i <= static_cast<int>(MAX_DIGITS));
	}
}

Widget::Widget(Widget&& orig) noexcept
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

Widget& Widget::operator=(Widget&& orig) noexcept
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

bool Widget::isActive() const
{
	return m_active;
}

void Widget::setActive(bool active)
{
	m_active = active;
}



void Widget::render() const
{
	if (m_active)
	{
		glBindVertexArray(m_ID);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
	}
}

Widget::Widget()
	: m_ID(Globals::INVALID_OPENGL_ID),
	m_positionsVBO(Globals::INVALID_OPENGL_ID),
	m_textCoordsVBO(Globals::INVALID_OPENGL_ID),
	m_active(false)
{
	glGenVertexArrays(1, &m_ID);

	glGenBuffers(1, &m_textCoordsVBO);
	glGenBuffers(1, &m_positionsVBO);
}

Widget::~Widget()
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

//Text
Text::Text()
	: Widget(),
	m_vertexCount(0),
	m_position()
{}

Text::Text(const std::array<glm::vec2, 6>& drawableRect)
	: Widget(),
	m_vertexCount(0),
	m_position()
{}

Text::Text(Text&& orig) noexcept
	: Widget(std::move(orig)),
	m_vertexCount(orig.m_vertexCount),
	m_position(orig.m_position)
{
	orig.m_vertexCount = 0;
}

Text& Text::operator=(Text&& orig) noexcept
{
	Widget::operator=(std::move(orig));
	m_vertexCount = orig.m_vertexCount;
	m_position = orig.m_position;

	orig.m_vertexCount = 0;

	return *this;
}

void Text::setPosition(const glm::vec2& position)
{
	m_position = position;
}

void Text::setText(int number, const std::unordered_map<char, int>& characterIDMap)
{
	assert(number > 0);

	std::vector<glm::vec2> positions;
	std::vector<glm::vec2> textCoords;

	if (number >= 10)
	{
		int i = 0;
		std::array<int, MAX_DIGITS> digits;
		collectDigits(digits, number, i);

		glm::ivec2 position = m_position;
		for (auto digit : digits)
		{
			std::array<glm::vec2, 6> quadCoords = getQuadCoords(position, { TEXT_SIZE, TEXT_SIZE });
			positions.insert(positions.begin() + positions.size(), quadCoords.cbegin(), quadCoords.cend());

			auto characterID = characterIDMap.find(convertDigit(digit));
			assert(characterID != characterIDMap.cend());

			std::array<glm::vec2, 6> characterTextCoords =
				getCharacterTextCoords(convertTo2DTextCoord(characterID->second, FONT_TEXTURE_TILESIZE, FONT_TEXTURE_COLUMNS, FONT_TEXTURE_SIZE), FONT_TEXTURE_COLUMNS);

			textCoords.insert(textCoords.begin() + textCoords.size(), characterTextCoords.cbegin(), characterTextCoords.cend());
			
			position.x += CHARACTER_SPACING;
		}
	}
	else
	{
		std::array<glm::vec2, 6> quadCoords = getQuadCoords(m_position, { TEXT_SIZE, TEXT_SIZE });
		positions.insert(positions.begin() + positions.size(), quadCoords.cbegin(), quadCoords.cend());

		auto characterID = characterIDMap.find(convertDigit(number));
		assert(characterID != characterIDMap.cend());

		std::array<glm::vec2, 6> characterTextCoords =
			getCharacterTextCoords(convertTo2DTextCoord(characterID->second, FONT_TEXTURE_TILESIZE, FONT_TEXTURE_COLUMNS, FONT_TEXTURE_SIZE), FONT_TEXTURE_COLUMNS);

		textCoords.insert(textCoords.begin() + textCoords.size(), characterTextCoords.cbegin(), characterTextCoords.cend());
	}

	setText(positions, textCoords);
}

void Text::render() const
{
	if (m_active)
	{
		assert(m_vertexCount > 0);

		glBindVertexArray(m_ID);
		glDrawArrays(GL_TRIANGLES, 0, m_vertexCount);
		glBindVertexArray(0);
	}
}

void Text::setText(const std::vector<glm::vec2>& positions, const std::vector<glm::vec2>& textCoords)
{
	assert(!positions.empty() && !textCoords.empty());

	m_vertexCount = positions.size();
	glBindVertexArray(m_ID);

	glBindBuffer(GL_ARRAY_BUFFER, m_positionsVBO);
	glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec2), positions.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (const void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, m_textCoordsVBO);
	glBufferData(GL_ARRAY_BUFFER, textCoords.size() * sizeof(glm::vec2), textCoords.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (const void*)(0));

	glBindVertexArray(0);
}

//Image
Image::Image()
	: Widget()
{}

void Image::setPosition(const std::array<glm::vec2, 6>& quadCoords) const
{
	glBindVertexArray(m_ID);
	glBindBuffer(GL_ARRAY_BUFFER, m_positionsVBO);
	glBufferData(GL_ARRAY_BUFFER, quadCoords.size() * sizeof(glm::vec2), quadCoords.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (const void*)(0));

	glBindVertexArray(0);
}

void Image::setTextureRect(const std::array<glm::vec2, 6>& drawableRect) const
{
	glBindVertexArray(m_ID);
	glBindBuffer(GL_ARRAY_BUFFER, m_textCoordsVBO);
	glBufferData(GL_ARRAY_BUFFER, drawableRect.size() * sizeof(glm::vec2), drawableRect.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (const void*)(0));

	glBindVertexArray(0);
}

void Image::setTextureRect(const std::array<glm::vec3, 6>& drawableRect) const
{
	glBindVertexArray(m_ID);
	glBindBuffer(GL_ARRAY_BUFFER, m_textCoordsVBO);
	glBufferData(GL_ARRAY_BUFFER, drawableRect.size() * sizeof(glm::vec3), drawableRect.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (const void*)(0));

	glBindVertexArray(0);
}

//GUI
Gui::Gui()
	: m_items(),
	m_itemQuantityText(),
	m_characterIDMap(),
	m_toolbar(),
	m_selectionBox()
{
	//Items
	for (int i = 0; i < m_items.size(); ++i)
	{
		glm::vec2 position = getPositionOnHotbar(static_cast<eInventoryIndex>(i), INITIAL_ITEM_STARTING_POSITION);
		m_items[i].setPosition(getQuadCoords(position, ITEM_SIZE));
	}

	glm::vec2 position = ITEM_QUANTITY_STARTING_POSITION;
	//Item Quantity Text
	for (auto& itemQuantityText : m_itemQuantityText)
	{
		itemQuantityText.setPosition(position);

		position.x += 55.0f;
	}

	//Toolbar
	m_toolbar.setPosition(getQuadCoords(TOOLBAR_STARTING_POSITION, { 500, 48 }));
	m_toolbar.setTextureRect(toolbarTextCoords);
	m_toolbar.setActive(true);

	//Selection Box
	m_selectionBox.setPosition(getQuadCoords(TOOLBAR_STARTING_POSITION, { 52, 48 }));
	m_selectionBox.setTextureRect(selectionBoxTextCoords);
	m_selectionBox.setActive(true);

	m_characterIDMap.insert({ '0', 16 });
	m_characterIDMap.insert({ '1', 17 });
	m_characterIDMap.insert({ '2', 18 });
	m_characterIDMap.insert({ '3', 19 });
	m_characterIDMap.insert({ '4', 20 });
	m_characterIDMap.insert({ '5', 21 });
	m_characterIDMap.insert({ '6', 22 });
	m_characterIDMap.insert({ '7', 23 });
	m_characterIDMap.insert({ '8', 24 });
	m_characterIDMap.insert({ '9', 25 });
}

void Gui::addItem(eInventoryIndex hotbarIndex, eCubeType cubeType)
{
	assert(!m_items[static_cast<int>(hotbarIndex)].isActive());
	m_items[static_cast<int>(hotbarIndex)].setTextureRect(getTextCoords(getTextureLayer(cubeType)));
	m_items[static_cast<int>(hotbarIndex)].setActive(true);
}

void Gui::removeItem(eInventoryIndex hotbarIndex)
{
	assert(m_items[static_cast<int>(hotbarIndex)].isActive());
	m_items[static_cast<int>(hotbarIndex)].setActive(false);
}

void Gui::updateSelectionBox(eInventoryIndex selectedItem)
{
	glm::vec2 position = getPositionOnHotbar(selectedItem, TOOLBAR_STARTING_POSITION);
	m_selectionBox.setPosition(getQuadCoords(position, { 52, 48 }));
}

void Gui::updateItemQuantity(eInventoryIndex selectedItem, int quantity)
{
	assert(m_items[static_cast<int>(selectedItem)].isActive());

	if (quantity > 0)
	{
		m_itemQuantityText[static_cast<int>(selectedItem)].setText(quantity, m_characterIDMap);
		m_itemQuantityText[static_cast<int>(selectedItem)].setActive(true);
	}
	else
	{
		assert(m_itemQuantityText[static_cast<int>(selectedItem)].isActive());
		m_itemQuantityText[static_cast<int>(selectedItem)].setActive(false);
	}
}

void Gui::render(ShaderHandler& shaderHandler, const Texture& widgetTexture, const Texture& fontTexture) const
{
	shaderHandler.switchToShader(eShaderType::UIItem);
	for (const auto& item : m_items)
	{
		item.render();
	}

	shaderHandler.switchToShader(eShaderType::UIToolbar);
	widgetTexture.bind();

	m_toolbar.render();
	
	glDisable(GL_DEPTH_TEST);
	
	m_selectionBox.render();

	fontTexture.bind();
	shaderHandler.switchToShader(eShaderType::UIFont);
	for (const auto& text : m_itemQuantityText)
	{
		text.render();
	}
	
	glEnable(GL_DEPTH_TEST);
}