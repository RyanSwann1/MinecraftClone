#pragma once

#include "ObjectPool.h"
#include "Chunk.h"
#include "VertexArray.h"
#include "ObjectQueue.h"
#include <vector>
#include <unordered_map>
#include <mutex>
#include <SFML/Graphics.hpp>
#include <atomic>
#include <functional>

struct ChunkToAdd
{
	ChunkToAdd(float distanceFromCamera, const glm::ivec3& startingPosition);

	float distanceFromCamera;
	glm::ivec3 startingPosition;
};

struct Rectangle;
class Player;
class Frustum;
class ChunkManager : private NonCopyable, private NonMovable
{
public:
	ChunkManager();

	bool getHighestCubeAtPosition(const glm::vec3& playerPosition, glm::vec3& position) const;
	bool isCubeAtPosition(const glm::vec3& playerPosition) const;
	bool isCubeAtPosition(const glm::vec3& playerPosition, eCubeType& cubeType) const;
	bool isChunkAtPosition(const glm::vec3& position) const;

	bool placeCubeAtPosition(const glm::ivec3& placementPosition, eCubeType cubeType);
	bool destroyCubeAtPosition(const glm::ivec3& blockToDestroy, eCubeType& destroyedCubeType);
	void update(const Player& player, const sf::Window& window, std::atomic<bool>& resetGame, 
		std::mutex& chunkInteractionMutex, std::mutex& renderingMutex);

	void renderOpaque(const Frustum& frustum) const;
	void renderTransparent(const Frustum& frustum) const;

private:
	ObjectPool<Chunk> m_chunkPool;
	ObjectPool<VertexArray> m_chunkMeshPool;
	std::unordered_map<glm::ivec3, ObjectFromPool<Chunk>> m_chunks;
	std::unordered_map<glm::ivec3, ObjectFromPool<VertexArray>> m_chunkMeshes;
	std::vector<ChunkToAdd> m_chunksToAdd;
	ObjectQueue<ObjectQueuePositionNode> m_chunkMeshesToGenerateQueue;
	ObjectQueue<ObjectQueuePositionNode> m_deletionQueue;
	ObjectQueue<ObjectQueueObjectNode<ObjectFromPool<VertexArray>>> m_generatedChunkMeshQueue;
	ObjectQueue<ObjectQueueObjectNode<ObjectFromPool<Chunk>>> m_generatedChunkQueue;
	ObjectQueue<ObjectQueueObjectNode<std::reference_wrapper<VertexArray>>> m_chunkMeshRegenerationQueue;
	
	void deleteChunks(const glm::ivec3& playerPosition, const Rectangle& visibilityRect);
	void addChunks(const glm::ivec3& playerPosition);
	void clearQueues(const glm::ivec3& playerPosition, const Rectangle& visibilityRect);
	
	void handleChunkMeshesToGenerateQueue();
	void handleChunkMeshRegenerationQueue();
	void handleGeneratedChunkMeshQueue();
	void handleGeneratedChunkQueue();
};