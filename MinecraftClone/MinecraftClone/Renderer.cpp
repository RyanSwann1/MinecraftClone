#include "Renderer.h"
#include "Shader.h"
#include "VertexArray.h"
#include "IndexBuffer.h"
#include "glad.h"

void Renderer::clear() const
{
	glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::draw(const VertexArray& vertexArray, const IndexBuffer& indexBuffer, const Shader& shader) const
{
	//shader.bind();
	//vertexArray.bind();
	//indexBuffer.bind();

	glDrawElements(GL_TRIANGLES, indexBuffer.getCount(), GL_UNSIGNED_INT, nullptr);
}