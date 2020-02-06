#pragma once

class VertexArray;
class IndexBuffer;
class Shader;
class Renderer
{
public:
	void clear() const;
	void draw(const VertexArray& vertexArray, const IndexBuffer& indexBuffer, const Shader& shader) const;
};