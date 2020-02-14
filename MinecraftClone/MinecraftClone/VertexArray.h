#pragma once

struct VertexBuffer;
class VertexArray
{
public:
	VertexArray();
	~VertexArray();

	void addVertexBuffer(const VertexBuffer& vertexBuffer) const;
	void bind() const;
	void unbind() const;

private:
	unsigned int m_ID;
};