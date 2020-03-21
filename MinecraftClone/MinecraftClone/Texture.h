#pragma once

#include <string>
#include "NonCopyable.h"
#include "NonMovable.h"
#include "glm/glm.hpp"
#include <memory>
#include <vector>

enum class eCubeFaceID;
class Texture : private NonCopyable, private NonMovable
{
public:
	static std::unique_ptr<Texture> loadTexture(const std::string& name);
	~Texture();

	unsigned int getCurrentSlot() const;

	void bind(unsigned int slot);
	void unbind() const;

private:
	Texture();
	unsigned int m_currentSlot;
	unsigned int m_ID;
};