#pragma once

#include "ObjectPool.h"
#include "Chunk.h"
#include "VertexArray.h"
#include "glm/gtx/hash.hpp"
#include "ObjectQueue.h"
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

struct NeighbouringChunks : private NonCopyable, private NonMovable
{
	NeighbouringChunks(const Chunk& leftChunk, const Chunk& rightChunk,
		const Chunk& topChunk, const Chunk& bottomChunk);

	const Chunk& leftChunk;
	const Chunk& rightChunk;
	const Chunk& forwardChunk;
	const Chunk& backChunk;
};

struct GeneratedChunkMesh : public ObjectQueueNode<GeneratedChunkMesh>
{
	GeneratedChunkMesh(const glm::ivec3& position, ObjectFromPool<VertexArray>&& vertexArrayFromPool);
	GeneratedChunkMesh(GeneratedChunkMesh&&) noexcept;
	GeneratedChunkMesh& operator=(GeneratedChunkMesh&&) noexcept;

	ObjectFromPool<VertexArray> vertexArrayFromPool;
};

struct BoundingBox;
struct Frustum;
class VertexArray;
class ChunkManager : private NonCopyable, private NonMovable
{
public:
	ChunkManager(const glm::ivec3& playerPosition);

	glm::vec3 resolveCollision(const glm::vec3& playerPosition) const;
	bool isCubeAtPosition(const BoundingBox& playerAABB, const glm::ivec3& position) const;

	void update(const glm::vec3& cameraPosition, const sf::Window& window, std::atomic<bool>& resetGame, 
		std::mutex& cameraMutex, std::mutex& renderingMutex);
	void renderOpaque(const Frustum& frustum) const;
	void renderTransparent(const Frustum& frustum) const;

private:
	ObjectPool<Chunk> m_chunkPool;
	ObjectPool<VertexArray> m_vertexArrayPool;
	std::unordered_map<glm::ivec3, ObjectFromPool<Chunk>> m_chunks;
	std::unordered_map<glm::ivec3, ObjectFromPool<VertexArray>> m_VAOs;
	ObjectQueue<PositionNode> m_chunkMeshesToGenerateQueue;
	ObjectQueue<PositionNode> m_deletedChunksQueue;
	ObjectQueue<GeneratedChunkMesh> m_generatedChunkMeshesQueue;

	void deleteChunks(const glm::ivec3& playerPosition, std::mutex& renderingMutex);
	void addChunks(const glm::ivec3& playerPosition);
	void generateChunkMeshes();
};