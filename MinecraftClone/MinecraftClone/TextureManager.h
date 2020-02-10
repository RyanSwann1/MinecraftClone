#pragma once

#include "Texture.h"
#include "NonCopyable.h"
#include <array>

enum class eTextureID
{
	Grass = 0,
	GrassSide,
	Dirt,
	Total
};

class TextureManager : private NonCopyable
{
public:
	TextureManager();

	bool addTexture(const std::string& name);

private:
	std::array<std::unique_ptr<Texture>, static_cast<size_t>(eTextureID::Total)> m_textures;
};