#pragma once

class IndexBuffer
{
public:
	IndexBuffer(const unsigned int* data, unsigned int count);
	~IndexBuffer();

	unsigned int getCount() const;

	void bind() const;
	void unbind() const;

private:
	unsigned int m_rendererID;
	unsigned int m_count;
};