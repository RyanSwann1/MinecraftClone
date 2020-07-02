#pragma once

#include "NonCopyable.h"
#include "NonMovable.h"
#include "Globals.h"
#include "glm/glm.hpp"
#include <vector>
#include <unordered_map>

constexpr size_t MAX_DIGITS = 2;

class Widget : private NonCopyable
{
public:
	Widget(Widget&&) noexcept;
	Widget& operator=(Widget&&) noexcept;
	const glm::vec2& getPosition() const;
	const glm::vec2& getScale() const;
	bool isActive() const;

	void setActive(bool active);
	void setTextureRect(const std::array<glm::vec2, 6>& drawableRect);
	void setPosition(const glm::vec2& position);
	void setScale(const glm::vec2& scale);
	virtual void render() const;

protected:
	Widget();
	~Widget();

	unsigned int m_ID;
	unsigned int m_positionsVBO;
	unsigned int m_textCoordsVBO;
	bool m_active;
	glm::vec2 m_position;
	glm::vec2 m_scale;
};

class Text : public Widget
{
public:	
	Text();
	Text(const std::array<glm::vec2, 6>& drawableRect);

	void setText(const std::array<int, MAX_DIGITS>& number, glm::vec2 position, 
		const std::unordered_map<char, int>& characterIDMap);

	void setText(int number, const std::unordered_map<char, int>& characterIDMap);
	
	void setText(const std::string& text, glm::vec2 position);
	void render() const override;

private:
	int m_size;
	
	void setText(const std::vector<glm::vec2>& positions, const std::vector<glm::vec2>& textCoords);
};

class Image : public Widget
{
public:
	Image();

	using Widget::setTextureRect;
	void setTextureRect(const std::array<glm::vec3, 6>& drawableRect);
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