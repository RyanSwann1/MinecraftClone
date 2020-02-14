#pragma once

#include "glm/glm.hpp"
#include <array>

class Texture;
struct VertexBuffer;
class Chunk
{
public:
	Chunk(glm::vec2 startingPosition, int elementArrayBufferIndex, VertexBuffer& vertexBuffer);

	const std::array<std::array<std::array<glm::vec3, 16>, 16>, 16> & getChunk() const;

private:
	std::array<std::array<std::array<glm::vec3, 16>, 16>, 16> m_chunk;

	void addCube(VertexBuffer& vertexBuffer, const Texture& texture, glm::vec3 startPosition, int elementArrayBufferIndex);
};