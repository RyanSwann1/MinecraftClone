#include "TextureManager.h"
#include <assert.h>

TextureManager::TextureManager()
{

}

bool TextureManager::addTexture(const std::string& name)
{


	/*auto iter = m_textures.find(name);
	assert(iter == m_textures.cend());
	if (iter == m_textures.cend())
	{
		std::unique_ptr<Texture> texture = Texture::loadTexture(name);
		if (texture)
		{
			m_textures[name] = std::move(texture);
			return true;
		}
	}*/

	return false;
}