#pragma once

#include <string>
#include <memory>

class Texture
{
public:
	~Texture();

	static std::unique_ptr<Texture> loadTexture(const std::string& name);

	unsigned int getCurrentSlot() const;
	unsigned int getID() const;

	void bind(unsigned int slot = 0);
	void unbind() const;

private:
	Texture();
	unsigned int m_currentSlot;
	unsigned int m_ID;
	int m_width;
	int m_height;
	int m_bytesPerPixel;
	unsigned char* m_localBuffer;
};