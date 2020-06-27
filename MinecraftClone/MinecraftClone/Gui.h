#pragma once

#include "NonCopyable.h"
#include "NonMovable.h"
#include "glm/glm.hpp"
#include <vector>
#include <array>

struct Rectangle;
class Image : private NonCopyable
{
public:
	Image();
	~Image();
	Image(Image&&) noexcept;
	Image& operator=(Image&&) noexcept;

	bool isActive() const;
	void setActive(bool active);
	void setTextureRect(const std::array<glm::vec3, 6>& drawableRect);

	void render() const;

private:
	unsigned int m_ID;
	unsigned int m_positionsVBO;
	unsigned int m_textCoordsVBO;
	bool m_active;
};

class Inventory;
enum class eCubeType;
enum class eHotbarIndex;
class ShaderHandler;
class Gui : private NonCopyable, private NonMovable
{
public:
	Gui();
	~Gui();

	void addItem(eHotbarIndex hotbarIndex, eCubeType cubeType);
	void removeItem(eHotbarIndex hotbarIndex);
	void renderItems(ShaderHandler& shaderHandler) const;

	void renderToolbar(ShaderHandler& shaderHandler, glm::vec2 windowSize) const;
	void renderSelectionBox(ShaderHandler& shaderHandler, glm::vec2 windowSize, eHotbarIndex hotBarSelection) const;

private:
	std::array<Image, 8> m_items;

	unsigned int m_itemID;
	unsigned int m_itemPositionsVBO;
	unsigned int m_itemTextCoordsVBO;

	unsigned int m_toolbarID;
	unsigned int m_toolbarPostionsVBO;
	unsigned int m_toolbarTextCoordsVBO;
	
	unsigned int m_selectionID;
	unsigned int m_selectionPositionsVBO;
	unsigned int m_selectionTextCoordsVBO;
};