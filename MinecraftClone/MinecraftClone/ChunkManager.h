#pragma once

#include "NonCopyable.h"
#include "Chunk.h"
#include "glm/gtx/hash.hpp"
#include <vector>
#include <memory>
#include <unordered_map>
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
//https://www.hackerearth.com/practice/data-structures/hash-tables/basics-of-hash-tables/tutorial/

struct Camera;
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

	void generateInitialChunks(glm::vec3 playerPosition, std::unordered_map<glm::ivec2, VertexArray>& VAOs, const Texture& texture);
	void update(Rectangle& visibilityRect, std::unordered_map<glm::ivec2, VertexArray>& VAOs, Camera& camera, 
		const Texture& texture, const sf::Window& window);

private:
	std::unordered_map<glm::ivec3, Chunk> m_chunks;
	std::vector<glm::ivec2> m_chunkMeshRegenerateQueue;
	std::mutex m_mutex;

	void addCubeFace(VertexBuffer& vertexBuffer, const Texture& texture, CubeDetails cubeDetails, eCubeSide cubeSide,
		int& opaqueElementBufferIndex, int& transparentElementBufferIndex, glm::ivec3 cubePosition);

	bool isCubeAtPosition(glm::ivec3 position) const;
	bool isChunkAtPosition(glm::ivec3 position) const;

	void generateChunkMesh(VertexArray& vertexArray, const Texture& texture, const Chunk& chunk);

	void deleteChunks(const Rectangle& visibilityRect, std::unordered_map<glm::ivec2, VertexArray>& VAOs);
	void addChunks(const Rectangle& visibilityRect, std::unordered_map<glm::ivec2, VertexArray>& VAOs, glm::vec3 playerPosition, const Texture& texture);
	void regenChunks(const Rectangle& visibilityRect, std::unordered_map<glm::ivec2, VertexArray>& VAOs, glm::vec3 playerPosition, const Texture& texture);
};