#pragma once

#include "NonCopyable.h"
#include "Chunk.h"
#include <vector>

class Texture;
struct VertexBuffer;
class ChunkManager : private NonCopyable
{
public:
	ChunkManager();

	void generateChunks(glm::vec3 startingPosition);
	void generateChunkMeshes(VertexBuffer& vertexBuffer, const Texture& Texture, int& elementArrayBufferIndex) const;

private:
	std::vector<Chunk> m_chunks;

	void addCube(VertexBuffer& vertexBuffer, const Texture& texture, glm::vec3 startPosition, int& elementArrayBufferIndex) const;
	bool isBlockAir(glm::vec3 position) const;
};