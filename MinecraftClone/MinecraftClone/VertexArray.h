#pragma once

class VertexBufferLayout;
class VertexBuffer;
class VertexArray
{
public:
	VertexArray();
	~VertexArray();

	void bind() const;
	void unbind() const;

	void addBuffer(const VertexBuffer& vertexBuffer, const VertexBufferLayout layout);

private:
	unsigned int m_rendererID;
};