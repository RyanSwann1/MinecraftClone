#pragma once

#include "NonCopyable.h"
#include "NonMovable.h"
#include "glm/glm.hpp"

enum class eTextureLayer;
enum class eCubeType;
class ShaderHandler;
class Gui : private NonCopyable, private NonMovable
{
public:
	Gui();
	~Gui();

	void drawSprite(eCubeType cubeType, ShaderHandler& shaderHandler, glm::vec2 windowSize) const;

private:
	unsigned int m_ID;
	unsigned int m_positionsVBO;
	unsigned int m_textCoordsVBO;
};