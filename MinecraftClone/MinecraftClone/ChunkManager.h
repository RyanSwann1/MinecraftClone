#pragma once

#include "ObjectPool.h"
#include "Chunk.h"
#include "VertexArray.h"
#include "ObjectQueue.h"
#include "ChunkMeshRegenerationQueue.h"
#include "GeneratedChunkMeshQueue.h"
#include "GeneratedChunkQueue.h"
#include "ChunkMeshesToGenerateQueue.h"
#include <vector>
#include <memory>
#include <unordered_map>
#include <mutex>
#include <SFML/Graphics.hpp>
#include <atomic>

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

struct Rectangle;
class Player;
struct Frustum;
class VertexArray;
class ChunkManager : private NonCopyable, private NonMovable
{
public:
	ChunkManager();

	glm::ivec3 getHighestCubeAtPosition(const glm::vec3& playerPosition) const;
	bool isCubeAtPosition(const glm::vec3& playerPosition) const;
	bool isCubeAtPosition(const glm::vec3& playerPosition, eCubeType& cubeType) const;
	bool isChunkAtPosition(const glm::vec3& position) const;

	bool placeCubeAtPosition(const glm::ivec3& placementPosition);
	bool destroyCubeAtPosition(const glm::ivec3& blockToDestroy);

	void update(const Player& player, const sf::Window& window, std::atomic<bool>& resetGame, 
		std::mutex& playerMutex, std::mutex& renderingMutex);
	void renderOpaque(const Frustum& frustum) const;
	void renderTransparent(const Frustum& frustum) const;

private:
	ObjectPool<Chunk> m_chunkPool;
	ObjectPool<VertexArray> m_chunkMeshPool;
	std::unordered_map<glm::ivec3, ObjectFromPool<Chunk>> m_chunks;
	std::unordered_map<glm::ivec3, ObjectFromPool<VertexArray>> m_chunkMeshes;
	ChunkMeshesToGenerateQueue m_chunkMeshesToGenerateQueue;
	ObjectQueue<PositionNode> m_deletionQueue;
	GeneratedChunkMeshQueue m_generatedChunkMeshesQueue;
	GeneratedChunkQueue m_generatedChunkQueue;
	ChunkMeshRegenerationQueue m_chunkMeshRegenerationQueue;

	void deleteChunks(const glm::ivec3& playerPosition, std::mutex& renderingMutex, const Rectangle& visibilityRect);
	void addChunks(const glm::ivec3& playerPosition);
	void clearQueues(const glm::ivec3& playerPosition, const Rectangle& visibilityRect);
};