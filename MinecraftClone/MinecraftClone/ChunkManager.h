#pragma once

#include "NonCopyable.h"
#include "Chunk.h"
#include <vector>
#include <queue>

//1. Perlin Noise
//2. Dynamic Generation
//3. Dynamic Destruction

//Dyanmic Chunk Generation
//https://gamedev.stackexchange.com/questions/173820/how-should-i-store-loaded-world-chunks-in-ram-for-my-game-similar-to-minecraft
//https://gamedev.stackexchange.com/questions/37911/how-to-deal-with-large-open-worlds

//https://www.reddit.com/r/VoxelGameDev/comments/4xsy0d/resources_for_terrain_generation_using/
//http://accidentalnoise.sourceforge.net/minecraftworlds.html
//https://learninggeekblog.wordpress.com/2013/04/25/voxel-engine-part-2/

struct Rectangle;
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
	void generateInitialChunks(glm::vec3 startingPosition, int chunkCount, std::vector<VertexArray>& VAOs, std::vector<VertexBuffer>& VBOs);
	void generateChunkMeshes(std::vector<VertexArray>& VAOs, std::vector<VertexBuffer>& VBOs, const Texture& Texture) const;

	void handleChunkMeshRegenerationQueue(std::vector<VertexArray>& VAOs, std::vector<VertexBuffer>& VBOs, const Texture& Texture);

	void update(const Rectangle& visibilityRect, std::vector<VertexArray>& VAOs, std::vector<VertexBuffer>& VBOs, glm::vec3 playerPosition, const Texture& texture);
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