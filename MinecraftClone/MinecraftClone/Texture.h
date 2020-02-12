#pragma once

#include <string>
#include <memory>

class Texture
{
public:
	static std::unique_ptr<Texture> loadTexture(const std::string& name, int rows);
	~Texture();
	unsigned int getCurrentSlot() const;
	unsigned int getID() const;

	int getXOffSet(int index) const;
	int getYOffSet(int index) const;

	void bind(unsigned int slot = 0);
	void unbind() const;

private:
	Texture(int rows);
	unsigned int m_currentSlot;
	unsigned int m_ID;

	int m_rows;
};