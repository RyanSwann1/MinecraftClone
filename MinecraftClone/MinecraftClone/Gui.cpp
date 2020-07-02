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

//sf::IntRect Texture::getFrameRect(int tileID) const
//{
//	Rect (T rectLeft, T rectTop, T rectWidth, T rectHeight)
//	return sf::IntRect(tileID % m_columns * m_tileSize.x, tileID / m_columns * m_tileSize.x, m_tileSize.x, m_tileSize.y);
//}

namespace
{
	constexpr int TEXT_SIZE = 32;
	constexpr int CHARACTER_SPACING = TEXT_SIZE / 2;
	constexpr int FONT_TEXTURE_COLUMNS = 8;
	constexpr glm::vec2 FONT_TEXTURE_TILESIZE = { 32, 32 };
	constexpr glm::vec2 FONT_TEXTURE_SIZE = { 256, 256 };

	std::array<glm::vec2, 6> getQuadCoords(const glm::vec2& position)
	{
		return {
			position, 
			glm::vec2(position.x + TEXT_SIZE, position.y),
			glm::vec2(position.x + TEXT_SIZE, position.y + TEXT_SIZE),
			glm::vec2(position.x + TEXT_SIZE, position.y + TEXT_SIZE),
			glm::vec2(position.x, position.y + TEXT_SIZE),
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

	glm::vec2 convertTo2DTextCoord(int i)
	{
		assert(i < 8 * 8);

		glm::vec2 position(i % 8, i / 8);
		return { 8 - position.x, 8 - position.y };
	}


	constexpr std::array<glm::vec2, 6> QUAD_COORDS =
	{
		glm::vec2(0, 0),
		glm::vec2(1, 0),
		glm::vec2(1, 1),
		glm::vec2(1, 1),
		glm::vec2(0, 1),
		glm::vec2(0, 0)
	};

	constexpr std::array<glm::vec2, 6> CHARACTER_ONE =
	{
		glm::vec2(0.125f, 0.375f),
		glm::vec2(0.25f, 0.375f),
		glm::vec2(0.25f, 0.25f),
		glm::vec2(0.25f, 0.25f),
		glm::vec2(0.125f, 0.25f),
		glm::vec2(0.125f, 0.375f)
	};

	constexpr std::array<glm::vec2, 6> CHARACTER_TWO =
	{
		glm::vec2(0.25f, 0.375f),
		glm::vec2(0.375f, 0.375f),
		glm::vec2(0.375f, 0.25f),
		glm::vec2(0.375f, 0.25f),
		glm::vec2(0.25f, 0.25f),
		glm::vec2(0.25f, 0.375f)
	};

	constexpr std::array<glm::vec2, 6> CHARACTER_THREE =
	{
		glm::vec2(0.25f, 0.375f),
		glm::vec2(0.375f, 0.375f),
		glm::vec2(0.375f, 0.25f),
		glm::vec2(0.375f, 0.25f),
		glm::vec2(0.25f, 0.25f),
		glm::vec2(0.25f, 0.375f)
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
		glm::vec2(0.1066f, .828125f),
		glm::vec2(0.1066f, 0.9101f),
		glm::vec2(0.1066f, 0.9101f),
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
	m_active(orig.m_active),
	m_position(orig.m_position),
	m_scale(orig.m_scale)
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
	m_position = orig.m_position;
	m_scale = orig.m_scale;

	orig.m_ID = Globals::INVALID_OPENGL_ID;
	orig.m_positionsVBO = Globals::INVALID_OPENGL_ID;
	orig.m_textCoordsVBO = Globals::INVALID_OPENGL_ID;
	orig.m_active = false;

	return *this;
}

//Widget
const glm::vec2& Widget::getPosition() const
{
	return m_position;
}

const glm::vec2& Widget::getScale() const
{
	return m_scale;
}

bool Widget::isActive() const
{
	return m_active;
}

void Widget::setActive(bool active)
{
	m_active = active;
}

void Widget::setTextureRect(const std::array<glm::vec2, 6>& drawableRect)
{
	glBindVertexArray(m_ID);
	glBindBuffer(GL_ARRAY_BUFFER, m_textCoordsVBO);
	glBufferData(GL_ARRAY_BUFFER, drawableRect.size() * sizeof(glm::vec2), drawableRect.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (const void*)(0));

	glBindVertexArray(0);
}

void Widget::setPosition(const glm::vec2& position)
{
	m_position = position;
}

void Widget::setScale(const glm::vec2& scale)
{
	m_scale = scale;
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
	m_active(false),
	m_position(),
	m_scale()
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
	m_size(0)
{}

Text::Text(const std::array<glm::vec2, 6>& drawableRect)
	: Widget(),
	m_size(0)
{}

void Text::setText(const std::array<int, MAX_DIGITS>& number, glm::vec2 position, const std::unordered_map<char, int>& characterIDMap)
{
	std::vector<glm::vec2> positions;
	std::vector<glm::vec2> textCoords;

	for (auto i : number)
	{
		std::array<glm::vec2, 6> quadCoords = getQuadCoords(position);
		positions.insert(positions.begin() + positions.size(), quadCoords.cbegin(), quadCoords.cend());

		auto characterID = characterIDMap.find(convertDigit(i));
		assert(characterID != characterIDMap.cend());

		glm::vec2 position =
			convertTo2DTextCoord(characterID->second, FONT_TEXTURE_TILESIZE, FONT_TEXTURE_COLUMNS, FONT_TEXTURE_SIZE);
		
		std::array<glm::vec2, 6> characterTextCoords = 
			getCharacterTextCoords(position, FONT_TEXTURE_COLUMNS);
		
		textCoords.insert(textCoords.begin() + textCoords.size(), characterTextCoords.cbegin(), characterTextCoords.cend());
		
		position.x += CHARACTER_SPACING;
	}

	setText(positions, textCoords);
}

void Text::setText(int number, const std::unordered_map<char, int>& characterIDMap)
{
	std::vector<glm::vec2> positions;
	std::vector<glm::vec2> textCoords;

	std::array<glm::vec2, 6> quadCoords = getQuadCoords(m_position);
	positions.insert(positions.begin() + positions.size(), quadCoords.cbegin(), quadCoords.cend());

	auto characterID = characterIDMap.find(convertDigit(number));
	assert(characterID != characterIDMap.cend());

	std::array<glm::vec2, 6> characterTextCoords = 
		getCharacterTextCoords(convertTo2DTextCoord(characterID->second, FONT_TEXTURE_TILESIZE, FONT_TEXTURE_COLUMNS, FONT_TEXTURE_SIZE), FONT_TEXTURE_COLUMNS);

	textCoords.insert(textCoords.begin() + textCoords.size(), characterTextCoords.cbegin(), characterTextCoords.cend());
	
	setText(positions, textCoords);
}

void Text::setText(const std::string& text, glm::vec2 position)
{
	std::vector<glm::vec2> positions;
	std::vector<glm::vec2> textCoords;

	for (auto i : text)
	{
		if (i != ' ')
		{
			std::array<glm::vec2, 6> quadCoords = getQuadCoords(position);
			positions.insert(positions.begin() + positions.size(), quadCoords.cbegin(), quadCoords.cend());

			std::array<glm::vec2, 6> characterTextCoords = CHARACTER_TWO;
			textCoords.insert(textCoords.begin() + textCoords.size(), characterTextCoords.cbegin(), characterTextCoords.cend());
		}

		position.x += CHARACTER_SPACING;
	}

	setText(positions, textCoords);
}

void Text::render() const
{
	if (m_active)
	{
		assert(m_size > 0);

		glBindVertexArray(m_ID);
		glDrawArrays(GL_TRIANGLES, 0, m_size);
		glBindVertexArray(0);
	}
}

void Text::setText(const std::vector<glm::vec2>& positions, const std::vector<glm::vec2>& textCoords)
{
	assert(!positions.empty() && !textCoords.empty());

	m_size = positions.size();
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
{
	glBindVertexArray(m_ID);

	glBindBuffer(GL_ARRAY_BUFFER, m_positionsVBO);
	glBufferData(GL_ARRAY_BUFFER, QUAD_COORDS.size() * sizeof(glm::vec2), QUAD_COORDS.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (const void*)0);

	glBindVertexArray(0);
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

//GUI
Gui::Gui()
	: m_items(),
	m_characterIDMap(),
	m_toolbar(),
	m_selectionBox()
{
	//Items
	for (int i = 0; i < m_items.size(); ++i)
	{
		m_items[i].setPosition(getPositionOnHotbar(static_cast<eInventoryIndex>(i), { 250, 250 }));
		m_items[i].setScale({ 50.0f, 50.0f });
	}

	glm::vec2 position{ 150.0f, 150.0f };
	//Item Quantity Text
	for (auto& itemQuantityText : m_itemQuantityText)
	{
		itemQuantityText.setPosition(position);

		position.x += 25.0f;
	}

	//Toolbar
	m_toolbar.setPosition({ 700.0f, 1025.f });
	m_toolbar.setScale({ 400.0f, 50.0f });
	m_toolbar.setTextureRect(toolbarTextCoords);
	m_toolbar.setActive(true);

	//Selection Box
	m_selectionBox.setPosition({ 670.0f, 950.0f });
	m_selectionBox.setScale({ 30.0f, 30.0f });
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
	m_selectionBox.setPosition(getPositionOnHotbar(selectedItem, { 670.0f, 950.0f }));
}

void Gui::updateItemQuantity(eInventoryIndex selectedItem, int quantity)
{
	assert(m_items[static_cast<int>(selectedItem)].isActive());

	if (quantity >= 10)
	{
		//int i = 0;
		//std::array<int, MAX_DIGITS> digits;
		//collectDigits(digits, quantity, i);
		//switch (digits[0])
		//{
		//default:
		//	break;
		//}
	}
	else if(quantity >= 1)
	{
		m_itemQuantityText[static_cast<int>(selectedItem)].setText(quantity, m_characterIDMap);
		m_itemQuantityText[static_cast<int>(selectedItem)].setActive(true);
	}
	else
	{
		m_itemQuantityText[static_cast<int>(selectedItem)].setActive(false);
	}
}

void Gui::render(ShaderHandler& shaderHandler, const Texture& widgetTexture, const Texture& fontTexture) const
{
	shaderHandler.switchToShader(eShaderType::UIItem);
	for (const auto& item : m_items)
	{
		glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(item.getPosition(), 0.0f));
		model = glm::scale(model, glm::vec3(item.getScale(), 1.0f));
		shaderHandler.setUniformMat4f(eShaderType::UIItem, "uModel", model);

		item.render();
	}

	shaderHandler.switchToShader(eShaderType::UIToolbar);
	widgetTexture.bind(0);

	{
		glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(m_toolbar.getPosition(), 0.0f));
		model = glm::scale(model, glm::vec3(m_toolbar.getScale(), 1.0f));
		shaderHandler.setUniformMat4f(eShaderType::UIToolbar, "uModel", model);

		m_toolbar.render();
	}

	{
		glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(m_selectionBox.getPosition(), 0.0f));
		model = glm::scale(model, glm::vec3(m_selectionBox.getScale(), 1.0f));
		shaderHandler.setUniformMat4f(eShaderType::UIToolbar, "uModel", model);

		m_selectionBox.render();
	}

	fontTexture.bind(0);
	shaderHandler.switchToShader(eShaderType::UIFont);
	for (const auto& text : m_itemQuantityText)
	{
		text.render();
	}
}