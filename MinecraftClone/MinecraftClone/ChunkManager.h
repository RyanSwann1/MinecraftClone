#pragma once

#include "NonCopyable.h"
#include "Chunk.h"
#include "glm/gtx/hash.hpp"
#include <vector>
#include <memory>
#include <unordered_map>
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

//https://www.reddit.com/r/VoxelGameDev/comments/btt0hz/octrees_and_chunks/

//https://www.reddit.com/r/proceduralgeneration/comments/2gy05g/creating_biomes_in_an_infinite_world_like_in/
//https://www.seedofandromeda.com/blogs/1-creating-a-region-file-system-for-a-voxel-game
//https://www.reddit.com/r/VoxelGameDev/comments/95hqrb/how_to_get_started_with_terrain_generation/

//https://algs4.cs.princeton.edu/34hash/
//https://www.hackerearth.com/practice/data-structures/hash-tables/basics-of-hash-tables/tutorial/


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
	void generateInitialChunks(glm::vec3 playerPosition, int chunkCount, std::vector<VertexArray>& VAOs, std::vector<VertexBuffer>& VBOs);
	void generateChunkMeshes(std::vector<VertexArray>& VAOs, std::vector<VertexBuffer>& VBOs, const Texture& Texture);
	void handleChunkMeshRegenerationQueue(std::vector<VertexArray>& VAOs, std::vector<VertexBuffer>& VBOs, const Texture& Texture);
	void update(const Rectangle& visibilityRect, std::vector<VertexArray>& VAOs, std::vector<VertexBuffer>& VBOs, glm::vec3 playerPosition, const Texture& texture);

private:
	std::unordered_map<glm::ivec2, std::shared_ptr<Chunk>> m_chunks;
	std::vector<glm::ivec2> m_chunkMeshRegenerateQueue;
	std::queue<std::shared_ptr<Chunk>> m_recycledChunks;

	void addCubeFace(VertexBuffer& vertexBuffer, const Texture& texture, CubeDetails cubeDetails, eCubeSide cubeSide, 
		int& elementArrayBufferIndex) const;
	bool isCubeAtPosition(glm::vec3 position) const;
	bool isCubeAtPosition(glm::ivec3 position) const;
	bool isChunkAtPosition(glm::ivec2 position) const;

	void generateChunkMesh(VertexArray& vertexArray, VertexBuffer& vertexBuffer, const Texture& texture, const Chunk& chunk);
	void handleAdjacentDestroyedBlock(glm::ivec3 position, const Chunk& owningChunk);
};