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
	static std::unique_ptr<TextureArray> create();
	~TextureArray();

	unsigned int getCurrentSlot() const;

	void bind() const;
	void unbind() const;

private:
	TextureArray(unsigned int ID);
	const unsigned int m_slot;
	const unsigned int m_ID;
};