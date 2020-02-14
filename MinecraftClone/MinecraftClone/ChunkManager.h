#pragma once

#include "NonCopyable.h"
#include "Chunk.h"
#include <vector>

enum class eCubeSide;
class Texture;
class VertexArray;
struct VertexBuffer;
class ChunkManager : private NonCopyable
{
public:
	ChunkManager();

	void generateChunks(glm::vec3 startingPosition, int chunkCount);
	void generateChunkMeshes(std::vector<VertexArray>& VAOs, std::vector<VertexBuffer>& VBOs, const Texture& Texture) const;

private:
	std::vector<Chunk> m_chunks;

	void addCube(VertexBuffer& vertexBuffer, const Texture& texture, glm::vec3 startPosition, int& elementArrayBufferIndex) const;
	void addCubeFace(VertexBuffer& vertexBuffer, const Texture& texture, glm::vec3 startPosition, eCubeSide cubeSide) const;
	bool isBlockAir(glm::vec3 position) const;
};