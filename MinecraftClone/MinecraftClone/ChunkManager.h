#pragma once

#include "NonCopyable.h"
#include "Chunk.h"
#include <vector>

enum class eCubeSide;
class Texture;
class VertexArray;
struct CubeDetails;
struct VertexBuffer;
class ChunkManager : private NonCopyable
{
public:
	ChunkManager();

	bool isPositionInChunk(glm::vec3 position) const;
	void generateChunks(glm::vec3 startingPosition, int chunkCount);
	void generateChunkMeshes(std::vector<VertexArray>& VAOs, std::vector<VertexBuffer>& VBOs, const Texture& Texture) const;

private:
	std::vector<Chunk> m_chunks;

	void addCubeFace(VertexBuffer& vertexBuffer, const Texture& texture, CubeDetails cubeDetails, eCubeSide cubeSide, 
		int& elementArrayBufferIndex) const;
	bool isCubeAtPosition(glm::vec3 position) const;

	void removeCubeFromChunk(glm::vec3 position);
};