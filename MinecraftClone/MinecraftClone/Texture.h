#pragma once

#include <string>
#include "NonCopyable.h"
#include "glm/glm.hpp"
#include <memory>
#include <vector>

enum class eTileID
{
	Grass = 0,
	DirtSide,
	Dirt
};

class Texture : private NonCopyable
{
public:
	static std::unique_ptr<Texture> loadTexture(const std::string& name);
	~Texture();
	unsigned int getCurrentSlot() const;
	unsigned int getID() const;

	void getTextCoords(eTileID tileID, std::vector<float>& textCoords) const;

	void bind(unsigned int slot = 0);
	void unbind() const;

private:
	Texture();
	unsigned int m_currentSlot;
	unsigned int m_ID;
};