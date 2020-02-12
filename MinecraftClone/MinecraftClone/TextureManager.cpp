#include "TextureManager.h"
#include <assert.h>

TextureManager::TextureManager()
{

}

bool TextureManager::addTexture(const std::string& name, eTextureID textureID)
{
	assert(!m_textures[static_cast<int>(textureID)]);
	if (!m_textures[static_cast<int>(textureID)])
	{
		std::unique_ptr<Texture> texture = Texture::loadTexture(name, 1);
		if (texture)
		{
			m_textures[static_cast<int>(textureID)] = std::move(texture);
			return true;
		}
	}

	return false;
}

const std::array<std::unique_ptr<Texture>, static_cast<size_t>(eTextureID::Total)>& TextureManager::getTextures() const
{
	return m_textures;
}
