#pragma once

#include "NonCopyable.h"
#include "NonMovable.h"
#include "glm/glm.hpp"
#include <string>
#include <memory>
#include <vector>

class TextureArray : private NonCopyable, private NonMovable
{
public:
	TextureArray();
	
	unsigned int getCurrentSlot() const;
	bool addTexture(const std::string& textureName);

	void bind() const;
	void unbind() const;

private:
	const glm::ivec2 m_textureSize;
	const unsigned int m_slot;
	unsigned int m_ID;
	int m_textureCount;


};