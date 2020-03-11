#pragma once

#include "ChunkPool.h"
#include "glm/gtx/hash.hpp"
#include "VertexArrayPool.h"
#include <vector>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <mutex>
#include <SFML/Graphics.hpp>

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
//https://www.hackerearth.com/practice/data-structures/hash-tables/basics-of-hash-tables/tutorial

//https://www.reddit.com/r/proceduralgeneration/comments/4eixfr/how_are_randomly_placed_structures_generated_in_a/

struct ChunkFromPool : private NonCopyable, private NonMovable
{
	ChunkFromPool(ChunkPool& chunkPool, const glm::ivec3& startingPosition)
		: chunk(chunkPool.getChunk(startingPosition))
	{}

	~ChunkFromPool()
	{
		chunk.release();
	}

	Chunk& chunk;
};

struct Camera;
struct Rectangle;
enum class eCubeSide;
class Texture;
class VertexArray;
struct CubeDetails;
class ChunkManager : private NonCopyable, private NonMovable
{
public:
	ChunkManager();

	std::unordered_map<glm::ivec3, VertexArrayFromPool>& getVAOs();

	void generateInitialChunks(const glm::vec3& playerPosition, const Texture& texture);
	void update(const Camera& camera, const sf::Window& window, const Texture& texture);

private:
	ChunkPool m_chunkPool;
	VertexArrayPool m_vertexArrayPool;
	std::unordered_map<glm::ivec3, VertexArrayFromPool> m_VAOs;
	std::unordered_map<glm::ivec3, ChunkFromPool> m_chunks;
	std::unordered_set<glm::ivec3> m_chunksToRegenerate;
	std::mutex m_mutex;

	void addCubeFace(VertexArray& vertexArray, eCubeType cubeType, eCubeSide cubeSide, const glm::ivec3& cubePosition,
		const Texture& texture);
	
	bool isCubeAtPosition(const glm::ivec3& position, const Chunk& chunk, eCubeType cubeType) const;
	bool isCubeAtPosition(const glm::ivec3& position, const Chunk& chunk) const;
	bool isChunkAtPosition(const glm::ivec3& position) const;

	void generateChunkMesh(VertexArray& vertexArray, const Chunk& chunk, const Texture& texture);

	void deleteChunks(const glm::ivec3& playerPosition);
	void addChunks(const glm::vec3& playerPosition, const Texture& texture);
	void regenChunks(const Texture& texture);

	const Chunk* getNeighbouringChunkAtPosition(const glm::ivec3& chunkStartingPosition) const;
};