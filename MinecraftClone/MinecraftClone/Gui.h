#pragma once

#include "NonCopyable.h"
#include "NonMovable.h"
#include "glm/glm.hpp"

enum class eTextureLayer;
class ShaderHandler;
class Gui : private NonCopyable, private NonMovable
{
public:
	Gui();
	~Gui();

	void drawSprite(eTextureLayer textureLayer, ShaderHandler& shaderHandler, glm::ivec2 windowSize) const;

private:
	unsigned int m_ID;
	unsigned int m_positionsVBO;
	unsigned int m_textCoordsVBO;
};