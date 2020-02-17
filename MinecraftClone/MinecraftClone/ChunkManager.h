#pragma once

#include "NonCopyable.h"
#include "Chunk.h"
#include <vector>
#include <queue>

enum class eCubeSide;
class Texture;
class VertexArray;
struct CubeDetails;
struct VertexBuffer;
class ChunkManager : private NonCopyable
{
public:
	ChunkManager();

	void removeCubeAtPosition(glm::vec3 cameraPosition, glm::vec3 rayCastPosition);
	void generateChunks(glm::vec3 startingPosition, int chunkCount, std::vector<VertexArray>& VAOs, std::vector<VertexBuffer>& VBOs);
	void generateChunkMeshes(std::vector<VertexArray>& VAOs, std::vector<VertexBuffer>& VBOs, const Texture& Texture) const;

	void handleChunkMeshRegenerationQueue(std::vector<VertexArray>& VAOs, std::vector<VertexBuffer>& VBOs, const Texture& Texture);

private:
	std::vector<Chunk> m_chunks;
	std::queue<const Chunk*> m_chunkMeshRegenerateQueue;

	void addCubeFace(VertexBuffer& vertexBuffer, const Texture& texture, CubeDetails cubeDetails, eCubeSide cubeSide, 
		int& elementArrayBufferIndex) const;
	bool isCubeAtPosition(glm::vec3 position) const;
	bool isCubeAtPosition(glm::ivec3 position) const;

	void generateChunkMesh(VertexArray& vertexArray, VertexBuffer& vertexBuffer, const Texture& texture, const Chunk& chunk) const;
	void handleAdjacentDestroyedBlock(glm::ivec3 position, const Chunk& owningChunk);
};