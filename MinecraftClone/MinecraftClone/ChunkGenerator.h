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
#include <atomic>
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


//https://github.com/gametutorials/tutorials/tree/master/OpenGL/Frustum%20Culling
//http://www.lighthouse3d.com/tutorials/view-frustum-culling/
//https://github.com/BSVino/MathForGameDevelopers/tree/frustum-culling

//https://github.com/Hopson97/MineCraft-One-Week-Challenge/blob/master/Source/Maths/Frustum.h
//https://github.com/Hopson97/MineCraft-One-Week-Challenge/blob/master/Source/Maths/Frustum.cpp

//http://www.lighthouse3d.com/tutorials/maths/

enum class eDirection;
enum class eCubeSide;
class VertexArray;
struct CubeDetails;
class ChunkGenerator : private NonCopyable, private NonMovable
{
	struct Regenerate : private NonCopyable, private NonMovable
	{
		Regenerate(const Chunk& chunk, VertexArrayFromPool&& vertexArrayFromPool);

		VertexArrayFromPool vertexArrayToRegenerate;
		const Chunk& chunkToRegenerate;
		bool regenerated;
	};

public:
	ChunkGenerator(const glm::ivec3& playerPosition);

	void update(const glm::vec3& cameraPosition, const sf::Window& window, std::atomic<bool>& resetGame, 
		std::mutex& cameraMutex, std::mutex& renderingMutex);
	void renderOpaque() const;
	void renderTransparent() const;

private:
	ChunkPool m_chunkPool;
	VertexArrayPool m_vertexArrayPool;
	std::unordered_map<glm::ivec3, VertexArrayFromPool> m_VAOs;
	std::unordered_map<glm::ivec3, Regenerate> m_regenerate;
	std::unordered_map<glm::ivec3, ChunkFromPool> m_chunks;

	const Chunk* getNeighbouringChunkAtPosition(const glm::ivec3& chunkStartingPosition, eDirection direction) const;
	bool isCubeAtPosition(const glm::ivec3& position, const Chunk& chunk) const;

	void addCubeFace(VertexBuffer& vertexBuffer, eCubeType cubeType, eCubeSide cubeSide, const glm::ivec3& cubePosition, bool transparent);
	void generateChunkMesh(VertexArray& vertexArray, const Chunk& chunk);
	void deleteChunks(const glm::ivec3& playerPosition, std::mutex& renderingMutex);
	void addChunks(const glm::vec3& playerPosition);
	void handleRegeneration(std::mutex& renderingMutex);
};