#pragma once

#include "NonCopyable.h"
#include "NonMovable.h"
#include "glm/glm.hpp"

enum class eTerrainTextureLayer;
enum class eCubeType;
enum class eHotBarSelection;
class ShaderHandler;
class Gui : private NonCopyable, private NonMovable
{
public:
	Gui();
	~Gui();

	void renderSprite(eCubeType cubeType, ShaderHandler& shaderHandler, glm::vec2 windowSize) const;
	void renderToolbar(ShaderHandler& shaderHandler, glm::vec2 windowSize) const;
	void renderSelectionBox(ShaderHandler& shaderHandler, glm::vec2 windowSize, eHotBarSelection hotBarSelection) const;

private:
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