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
#include <unordered_map>
#include <mutex>
#include <SFML/Graphics.hpp>
#include <atomic>

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

	bool placeCubeAtPosition(const glm::ivec3& placementPosition, eCubeType cubeType);
	bool destroyCubeAtPosition(const glm::ivec3& blockToDestroy, eCubeType& destroyedCubeType);

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
	
	void deleteChunks(const glm::ivec3& playerPosition, const Rectangle& visibilityRect);
	void addChunks(const glm::ivec3& playerPosition);
	void clearQueues(const glm::ivec3& playerPosition, const Rectangle& visibilityRect);
};