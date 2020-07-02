#pragma once

#include "NonCopyable.h"
#include "NonMovable.h"
#include "Globals.h"
#include "glm/glm.hpp"
#include <vector>
#include <unordered_map>

class Widget : private NonCopyable
{
public:
	Widget(Widget&&) noexcept;
	Widget& operator=(Widget&&) noexcept;

	bool isActive() const;

	void setActive(bool active);
	virtual void render() const;

protected:
	Widget();
	~Widget();

	unsigned int m_ID;
	unsigned int m_positionsVBO;
	unsigned int m_textCoordsVBO;
	bool m_active;
};

class Text : public Widget
{
public:	
	Text();
	Text(const std::array<glm::vec2, 6>& drawableRect);
	Text(Text&&) noexcept;
	Text& operator=(Text&&) noexcept;

	void setPosition(const glm::vec2& position);
	void setText(int number, const std::unordered_map<char, int>& characterIDMap);
	void render() const override;

private:
	int m_vertexCount;
	glm::vec2 m_position;

	void setText(const std::vector<glm::vec2>& positions, const std::vector<glm::vec2>& textCoords);
};

class Image : public Widget
{
public:
	Image();

	void setPosition(const std::array<glm::vec2, 6>& quadCoords) const;
	void setTextureRect(const std::array<glm::vec2, 6>& drawableRect) const;
	void setTextureRect(const std::array<glm::vec3, 6>& drawableRect) const;
};

class Texture;
enum class eCubeType;
enum class eInventoryIndex;
class ShaderHandler;
class Gui : private NonCopyable, private NonMovable
{
public:
	Gui();

	void addItem(eInventoryIndex hotbarIndex, eCubeType cubeType);
	void removeItem(eInventoryIndex hotbarIndex);
	void updateSelectionBox(eInventoryIndex selectedItemIndex);
	void updateItemQuantity(eInventoryIndex selectedItem, int quantity);

	void render(ShaderHandler& shaderHandler, const Texture& widgetTexture, const Texture& fontTexture) const;

private:
	std::array<Image, static_cast<size_t>(eInventoryIndex::Max) + 1> m_items;
	std::array<Text, static_cast<size_t>(eInventoryIndex::Max) + 1> m_itemQuantityText;
	std::unordered_map<char, int> m_characterIDMap;

	Image m_toolbar;
	Image m_selectionBox;
};