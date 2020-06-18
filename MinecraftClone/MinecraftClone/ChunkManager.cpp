#include "ChunkManager.h"
#include "Globals.h"
#include "VertexBuffer.h"
#include "VertexArray.h"
#include "CubeType.h"
#include "Rectangle.h"
#include "Player.h"
#include "Frustum.h"
#include "ChunkMeshGenerator.h"
#include "BoundingBox.h"
#include "NeighbouringChunks.h"
#include <deque>

namespace
{
	constexpr int THREAD_TRANSFER_PER_FRAME = 8;

	int getObjectPoolSize()
	{
		int x = 2 * Globals::VISIBILITY_DISTANCE / Globals::CHUNK_WIDTH + 1;
		int z = 2 * Globals::VISIBILITY_DISTANCE / Globals::CHUNK_DEPTH + 1;

		return x * z;
	}

	glm::ivec3 getClosestMiddlePosition(const glm::ivec3& position)
	{
		glm::ivec3 middlePosition = position;
		if (position.x % (Globals::CHUNK_WIDTH / 2) < 0)
		{
			middlePosition.x += std::abs(position.x % Globals::CHUNK_WIDTH / 2);
			middlePosition.x -= Globals::CHUNK_WIDTH / 2;
		}
		else if (position.x % (Globals::CHUNK_WIDTH / 2) > 0)
		{
			middlePosition.x -= std::abs(position.x % Globals::CHUNK_WIDTH / 2);
			middlePosition.x += Globals::CHUNK_WIDTH / 2;
		}
		if (position.z % (Globals::CHUNK_DEPTH / 2) < 0)
		{
			middlePosition.z += std::abs(position.z % Globals::CHUNK_DEPTH / 2);
			middlePosition.z -= Globals::CHUNK_DEPTH / 2;
		}
		else if (position.z % (Globals::CHUNK_DEPTH / 2) > 0)
		{
			middlePosition.z -= std::abs(position.z % Globals::CHUNK_DEPTH / 2);
			middlePosition.z += Globals::CHUNK_DEPTH / 2;
		}

		return { middlePosition.x, 0, middlePosition.z };
	}

	glm::ivec3 getClosestChunkStartingPosition(const glm::ivec3& position)
	{
		glm::ivec3 closestChunkStartingPosition = position;
		if (position.x % Globals::CHUNK_WIDTH < 0)
		{
			closestChunkStartingPosition.x += std::abs(position.x % Globals::CHUNK_WIDTH);
			closestChunkStartingPosition.x -= Globals::CHUNK_WIDTH;
		}
		else if (position.x % Globals::CHUNK_WIDTH > 0)
		{
			closestChunkStartingPosition.x -= std::abs(position.x % Globals::CHUNK_WIDTH);
		}
		if (position.z % Globals::CHUNK_DEPTH < 0)
		{
			closestChunkStartingPosition.z += std::abs(position.z % Globals::CHUNK_DEPTH);
			closestChunkStartingPosition.z -= Globals::CHUNK_DEPTH;
		}
		else if (position.z % Globals::CHUNK_DEPTH > 0)
		{
			closestChunkStartingPosition.z -= std::abs(position.z % Globals::CHUNK_DEPTH);
		}

		return { closestChunkStartingPosition.x, 0, closestChunkStartingPosition.z };
	}

	float getSqrMagnitude(const glm::ivec3& positionA, const glm::ivec3& positionB)
	{
		return glm::pow(positionA.x - positionB.x, 2) +
			glm::pow(positionA.y - positionB.y, 2) +
			glm::pow(positionA.z - positionB.z, 2);
	}
}

//ChunkGenerator
ChunkManager::ChunkManager()
	: m_chunkPool(getObjectPoolSize()),
	m_chunkMeshPool(getObjectPoolSize()),
	m_chunks(),
	m_chunkMeshes(),
	m_chunkMeshesToGenerateQueue(),
	m_deletionQueue(),
	m_generatedChunkMeshesQueue(),
	m_generatedChunkQueue(),
	m_chunkMeshRegenerationQueue()
{
	addChunks(Globals::PLAYER_STARTING_POSITION);
}

glm::ivec3 ChunkManager::getHighestCubeAtPosition(const glm::vec3& playerPosition) const
{
	glm::ivec3 closestChunkStartingPosition = getClosestChunkStartingPosition(playerPosition);
	auto chunk = m_chunks.find(closestChunkStartingPosition);
	assert(chunk != m_chunks.cend());

	return chunk->second.getObject()->getHighestCubeAtPosition(playerPosition);
}

bool ChunkManager::isCubeAtPosition(const glm::vec3& playerPosition) const
{
	glm::ivec3 closestChunkStartingPosition = getClosestChunkStartingPosition(playerPosition);
	auto chunk = m_chunks.find(closestChunkStartingPosition);
	assert(chunk != m_chunks.cend());

	return chunk->second.getObject()->isCubeAtPosition(playerPosition);
}

bool ChunkManager::isCubeAtPosition(const glm::vec3& playerPosition, eCubeType& cubeType) const
{
	glm::ivec3 closestChunkStartingPosition = getClosestChunkStartingPosition(playerPosition);
	auto chunk = m_chunks.find(closestChunkStartingPosition);
	assert(chunk != m_chunks.cend());

	if (chunk->second.getObject()->isCubeAtPosition(playerPosition))
	{
		cubeType = static_cast<eCubeType>(chunk->second.getObject()->getCubeDetailsWithoutBoundsCheck(playerPosition));
		return true;
	}

	return false;
}

bool ChunkManager::isChunkAtPosition(const glm::vec3& position) const
{
	glm::ivec3 closestChunkStartingPosition = getClosestChunkStartingPosition(position);
	auto chunk = m_chunks.find(closestChunkStartingPosition);
	
	return chunk != m_chunks.cend();
}

bool ChunkManager::placeCubeAtPosition(const glm::ivec3& placementPosition)
{
	glm::ivec3 chunkStartingPosition = getClosestChunkStartingPosition(placementPosition);
	auto chunk = m_chunks.find(chunkStartingPosition);
	assert(chunk != m_chunks.end());

	if (chunk->second.getObject()->addCubeAtPosition(placementPosition, getAllNeighbouringChunks(m_chunks, chunkStartingPosition)))
	{
		auto chunkMesh = m_chunkMeshes.find(chunkStartingPosition);
		assert(chunkMesh != m_chunkMeshes.end());

		if (!m_chunkMeshRegenerationQueue.contains(chunkStartingPosition))
		{
			m_chunkMeshRegenerationQueue.add({ chunkStartingPosition, *chunkMesh->second.getObject() });
		}

		//Left Chunk
		glm::ivec3 leftChunkStartingPosition = getClosestChunkStartingPosition({ placementPosition .x - 1, placementPosition.y, placementPosition.z });
		if (leftChunkStartingPosition != chunkStartingPosition && !m_chunkMeshRegenerationQueue.contains(leftChunkStartingPosition))
		{
			auto leftChunkMesh = m_chunkMeshes.find(leftChunkStartingPosition);
			assert(leftChunkMesh != m_chunkMeshes.end());
			m_chunkMeshRegenerationQueue.add({ leftChunkStartingPosition, *leftChunkMesh->second.getObject() });
		}

		//Right Chunk
		glm::ivec3 rightChunkStartingPosition = getClosestChunkStartingPosition({ placementPosition.x + 1, placementPosition.y, placementPosition.z });
		if (rightChunkStartingPosition != chunkStartingPosition && !m_chunkMeshRegenerationQueue.contains(rightChunkStartingPosition))
		{
			auto rightChunkMesh = m_chunkMeshes.find(rightChunkStartingPosition);
			assert(rightChunkMesh != m_chunkMeshes.end());
			m_chunkMeshRegenerationQueue.add({ rightChunkStartingPosition, *rightChunkMesh->second.getObject() });
		}

		//Forward Chunk
		glm::ivec3 forwardChunkStartingPosition = getClosestChunkStartingPosition({ placementPosition.x, placementPosition.y, placementPosition.z + 1 });
		if (forwardChunkStartingPosition != chunkStartingPosition && !m_chunkMeshRegenerationQueue.contains(forwardChunkStartingPosition))
		{
			auto forwardChunkMesh = m_chunkMeshes.find(forwardChunkStartingPosition);
			assert(forwardChunkMesh != m_chunkMeshes.end());
			m_chunkMeshRegenerationQueue.add({ forwardChunkStartingPosition, *forwardChunkMesh->second.getObject() });
		}

		//Back Chunk
		glm::ivec3 backChunkStartingPosition = getClosestChunkStartingPosition({ placementPosition.x, placementPosition.y, placementPosition.z - 1 });
		if (backChunkStartingPosition != chunkStartingPosition && !m_chunkMeshRegenerationQueue.contains(backChunkStartingPosition))
		{
			auto backChunkMesh = m_chunkMeshes.find(backChunkStartingPosition);
			assert(backChunkMesh != m_chunkMeshes.end());
			m_chunkMeshRegenerationQueue.add({ backChunkStartingPosition, *backChunkMesh->second.getObject() });
		}

		return true;
	}

	return false;
}

bool ChunkManager::destroyCubeAtPosition(const glm::ivec3& blockToDestroy)
{
	glm::ivec3 chunkStartingPosition = getClosestChunkStartingPosition(blockToDestroy);
	auto chunk = m_chunks.find(chunkStartingPosition);
	assert(chunk != m_chunks.end());
	
	if (chunk->second.getObject()->destroyCubeAtPosition(blockToDestroy))
	{
		auto chunkMesh = m_chunkMeshes.find(chunkStartingPosition);
		assert(chunkMesh != m_chunkMeshes.end());

		if (!m_chunkMeshRegenerationQueue.contains(chunkStartingPosition))
		{
			m_chunkMeshRegenerationQueue.add({ chunkStartingPosition, *chunkMesh->second.getObject() });
		}

		//Left Chunk
		glm::ivec3 leftChunkStartingPosition = getClosestChunkStartingPosition({ blockToDestroy.x - 1, blockToDestroy.y, blockToDestroy.z });
		if (leftChunkStartingPosition != chunkStartingPosition && !m_chunkMeshRegenerationQueue.contains(leftChunkStartingPosition))
		{
			auto leftChunkMesh = m_chunkMeshes.find(leftChunkStartingPosition);
			assert(leftChunkMesh != m_chunkMeshes.end());
			m_chunkMeshRegenerationQueue.add({ leftChunkStartingPosition, *leftChunkMesh->second.getObject() });
		}
		
		//Right Chunk
		glm::ivec3 rightChunkStartingPosition = getClosestChunkStartingPosition({ blockToDestroy.x + 1, blockToDestroy.y, blockToDestroy.z });
		if (rightChunkStartingPosition != chunkStartingPosition && !m_chunkMeshRegenerationQueue.contains(rightChunkStartingPosition))
		{
			auto rightChunkMesh = m_chunkMeshes.find(rightChunkStartingPosition);
			assert(rightChunkMesh != m_chunkMeshes.end());
			m_chunkMeshRegenerationQueue.add({ rightChunkStartingPosition, *rightChunkMesh->second.getObject() });
		}
		
		//Forward Chunk
		glm::ivec3 forwardChunkStartingPosition = getClosestChunkStartingPosition({ blockToDestroy.x, blockToDestroy.y, blockToDestroy.z + 1 });
		if (forwardChunkStartingPosition != chunkStartingPosition && !m_chunkMeshRegenerationQueue.contains(forwardChunkStartingPosition))
		{
			auto forwardChunkMesh = m_chunkMeshes.find(forwardChunkStartingPosition);
			assert(forwardChunkMesh != m_chunkMeshes.end());
			m_chunkMeshRegenerationQueue.add({ forwardChunkStartingPosition, *forwardChunkMesh->second.getObject() });
		}
		
		//Back Chunk
		glm::ivec3 backChunkStartingPosition = getClosestChunkStartingPosition({ blockToDestroy.x, blockToDestroy.y, blockToDestroy.z - 1});
		if (backChunkStartingPosition != chunkStartingPosition && !m_chunkMeshRegenerationQueue.contains(backChunkStartingPosition))
		{
			auto backChunkMesh = m_chunkMeshes.find(backChunkStartingPosition);
			assert(backChunkMesh != m_chunkMeshes.end());
			m_chunkMeshRegenerationQueue.add({ backChunkStartingPosition, *backChunkMesh->second.getObject() });
		}
		
		return true;
	}

	return false;
}

//Two threads acquire two locks in different order
void ChunkManager::update(const Player& player, const sf::Window& window, std::atomic<bool>& resetGame,
	std::mutex& playerMutex, std::mutex& renderingMutex)	
{
	while (!resetGame && window.isOpen())
	{
		std::unique_lock<std::mutex> playerLock(playerMutex);
		glm::ivec3 playerPosition = player.getPosition();
		playerLock.unlock();

		glm::ivec3 startingPosition = getClosestMiddlePosition(playerPosition);
		Rectangle visibilityRect(glm::vec2(startingPosition.x, startingPosition.z), Globals::VISIBILITY_DISTANCE);

		deleteChunks(playerPosition, renderingMutex, visibilityRect);
		addChunks(playerPosition);
		m_chunkMeshesToGenerateQueue.update(m_chunkMeshPool, m_chunks, m_generatedChunkMeshesQueue);
		clearQueues(playerPosition, visibilityRect);

		playerLock.lock();
		std::lock_guard<std::mutex> renderingLock(renderingMutex); 
		m_chunkMeshRegenerationQueue.update(m_chunks);
		for (int i = 0; i < THREAD_TRANSFER_PER_FRAME; ++i)
		{
			if (!m_deletionQueue.isEmpty())
			{
				const glm::ivec3& chunkStartingPosition = m_deletionQueue.front().getPosition();
				auto chunkMesh = m_chunkMeshes.find(chunkStartingPosition);
				if (chunkMesh != m_chunkMeshes.end())
				{
					m_chunkMeshes.erase(chunkMesh);
				}

				auto chunk = m_chunks.find(chunkStartingPosition);
				if (chunk != m_chunks.end())
				{
					m_chunks.erase(chunk);
				}

				m_deletionQueue.pop();
			}

			m_generatedChunkQueue.update(m_chunks);
			m_generatedChunkMeshesQueue.update(m_chunkMeshes);
		}
	}
}

void ChunkManager::renderOpaque(const Frustum& frustum) const
{
	for (const auto& chunkMesh : m_chunkMeshes)
	{
		if (chunkMesh.second.getObject()->m_opaqueVertexBuffer.bindToVAO)
		{
			chunkMesh.second.getObject()->attachOpaqueVBO();
		}

		if (chunkMesh.second.getObject()->m_opaqueVertexBuffer.displayable && frustum.isChunkInFustrum(chunkMesh.first))
		{
			chunkMesh.second.getObject()->bindOpaqueVAO();
			glDrawElements(GL_TRIANGLES, chunkMesh.second.getObject()->m_opaqueVertexBuffer.indicies.size(), GL_UNSIGNED_INT, nullptr);
		}
	}
}

void ChunkManager::renderTransparent(const Frustum& frustum) const
{
	for (const auto& chunkMesh : m_chunkMeshes)
	{
		if (chunkMesh.second.getObject()->m_transparentVertexBuffer.bindToVAO)
		{
			chunkMesh.second.getObject()->attachTransparentVBO();
		}

		if (chunkMesh.second.getObject()->m_transparentVertexBuffer.displayable && frustum.isChunkInFustrum(chunkMesh.first))
		{
			chunkMesh.second.getObject()->bindTransparentVAO();
			glDrawElements(GL_TRIANGLES, chunkMesh.second.getObject()->m_transparentVertexBuffer.indicies.size(), GL_UNSIGNED_INT, nullptr);
		}
	}
}

void ChunkManager::deleteChunks(const glm::ivec3& playerPosition, std::mutex& renderingMutex, const Rectangle& visibilityRect)
{
	for (auto chunk = m_chunks.begin(); chunk != m_chunks.end(); ++chunk)
	{
		const glm::ivec3& chunkStartingPosition = chunk->second.getObject()->getStartingPosition();
		if (!m_deletionQueue.contains(chunkStartingPosition) &&
			!visibilityRect.contains(chunk->second.getObject()->getAABB()))
		{
			m_deletionQueue.add({ chunkStartingPosition });
		}
	}
}

void ChunkManager::addChunks(const glm::ivec3& playerPosition)
{
	struct ChunkToAdd
	{
		ChunkToAdd(float distanceFromCamera, const glm::ivec3& startingPosition)
			: distanceFromCamera(distanceFromCamera),
			startingPosition(startingPosition)
		{}

		float distanceFromCamera;
		glm::ivec3 startingPosition;
	};

	std::vector<ChunkToAdd> chunksToAdd;
	glm::ivec3 startPosition = getClosestMiddlePosition(playerPosition);
	startPosition = getClosestChunkStartingPosition(startPosition);
	for (int z = startPosition.z - Globals::VISIBILITY_DISTANCE; z <= startPosition.z + Globals::VISIBILITY_DISTANCE; z += Globals::CHUNK_DEPTH)
	{
		for (int x = startPosition.x - Globals::VISIBILITY_DISTANCE; x <= startPosition.x + Globals::VISIBILITY_DISTANCE; x += Globals::CHUNK_WIDTH)
		{
			glm::ivec3 chunkStartingPosition(x, 0, z);
			if (m_chunks.find(chunkStartingPosition) == m_chunks.cend() && 
				m_chunkMeshes.find(chunkStartingPosition) == m_chunkMeshes.cend() && 
				!m_chunkMeshesToGenerateQueue.contains(chunkStartingPosition) &&
				!m_generatedChunkMeshesQueue.contains(chunkStartingPosition) && 
				!m_generatedChunkQueue.contains(chunkStartingPosition))
			{
				chunksToAdd.emplace_back(getSqrMagnitude(chunkStartingPosition, playerPosition), chunkStartingPosition);
			}
		}
	}

	if (!chunksToAdd.empty())
	{
		std::sort(chunksToAdd.begin(), chunksToAdd.end(), [](const auto& a, const auto& b)
		{
			return a.distanceFromCamera < b.distanceFromCamera;
		});

		for (const auto& chunkToAdd : chunksToAdd)
		{
			ObjectFromPool<Chunk> chunkFromPool = m_chunkPool.getNextAvailableObject();
			if (chunkFromPool.getObject())
			{
				chunkFromPool.getObject()->reuse(chunkToAdd.startingPosition);
				m_generatedChunkQueue.add({ chunkToAdd.startingPosition, std::move(chunkFromPool) });

				m_chunkMeshesToGenerateQueue.add({ chunkToAdd.startingPosition });
			}
		}
	}
}

void ChunkManager::clearQueues(const glm::ivec3& playerPosition, const Rectangle& visibilityRect)
{
	m_chunkMeshesToGenerateQueue.removeOutOfBoundsElements(visibilityRect);
	m_generatedChunkMeshesQueue.removeOutOfBoundsElements(visibilityRect);
	m_generatedChunkQueue.removeOutOfBoundsElements(visibilityRect);
}