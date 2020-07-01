#pragma once

#include "NonCopyable.h"
#include "NonMovable.h"
#include "Globals.h"
#include "glm/glm.hpp"
#include <vector>

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

	void setText(const std::string& text, glm::vec2 position);
	void render() const override;

private:
	int m_size;
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

	Image m_toolbar;
	Image m_selectionBox;
	Text m_text;
};