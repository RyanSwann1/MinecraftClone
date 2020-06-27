#pragma once

#include "NonCopyable.h"
#include "NonMovable.h"
#include "Globals.h"
#include "glm/glm.hpp"
#include <vector>

struct Rectangle;
class Image : private NonCopyable
{
public:
	Image();
	~Image();
	Image(Image&&) noexcept;
	Image& operator=(Image&&) noexcept;

	const glm::vec2& getPosition() const;
	const glm::vec2& getScale() const;
	bool isActive() const;
	
	void setActive(bool active);
	void setTextureRect(const std::array<glm::vec3, 6>& drawableRect);
	void setTextureRect(const std::array<glm::vec2, 6>& drawableRect);
	void setPosition(const glm::vec2& position);
	void setScale(const glm::vec2& scale);

	void render() const;

private:
	unsigned int m_ID;
	unsigned int m_positionsVBO;
	unsigned int m_textCoordsVBO;
	bool m_active;
	glm::vec2 m_position;
	glm::vec2 m_scale;
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

	void render(ShaderHandler& shaderHandler, const Texture& widgetTexture) const;

private:
	std::array<Image, static_cast<size_t>(eInventoryIndex::Max) + 1> m_items;
	Image m_toolbar;
	Image m_selectionBox;
};