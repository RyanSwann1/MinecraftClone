#pragma once

#include <string>
#include <memory>

class Texture
{
public:
	static std::unique_ptr<Texture> loadTexture(const std::string& name);
	~Texture();
	unsigned int getCurrentSlot() const;
	unsigned int getID() const;

	void bind(unsigned int slot = 0);
	void unbind() const;

private:
	Texture();
	unsigned int m_currentSlot;
	unsigned int m_ID;
};