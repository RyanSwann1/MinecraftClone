#include "ChunkManager.h"
#include "Utilities.h"
#include "VertexBuffer.h"
#include "VertexArray.h"
#include "CubeType.h"
#include "Rectangle.h"
#include "Player.h"
#include "Frustum.h"
#include "ChunkMeshGenerator.h"
#include "BoundingBox.h"
#include <iostream>
#include <deque>

namespace
{
	constexpr int THREAD_TRANSFER_PER_FRAME = 1;

	int getObjectPoolSize()
	{
		int x = 2 * Utilities::VISIBILITY_DISTANCE / Utilities::CHUNK_WIDTH + 1;
		int z = 2 * Utilities::VISIBILITY_DISTANCE / Utilities::CHUNK_DEPTH + 1;

		return x * z;
	}

	void getClosestMiddlePosition(glm::ivec3& position)
	{
		if (position.x % (Utilities::CHUNK_WIDTH / 2) < 0)
		{
			position.x += std::abs(position.x % Utilities::CHUNK_WIDTH / 2);
			position.x -= Utilities::CHUNK_WIDTH / 2;
		}
		else if (position.x % (Utilities::CHUNK_WIDTH / 2) > 0)
		{
			position.x -= std::abs(position.x % Utilities::CHUNK_WIDTH / 2);
			position.x += Utilities::CHUNK_WIDTH / 2;
		}
		if (position.z % (Utilities::CHUNK_DEPTH / 2) < 0)
		{
			position.z += std::abs(position.z % Utilities::CHUNK_DEPTH / 2);
			position.z -= Utilities::CHUNK_DEPTH / 2;
		}
		else if (position.z % (Utilities::CHUNK_DEPTH / 2) > 0)
		{
			position.z -= std::abs(position.z % Utilities::CHUNK_DEPTH / 2);
			position.z += Utilities::CHUNK_DEPTH / 2;
		}
	}

	void getClosestChunkStartingPosition(glm::ivec3& position)
	{
		if (position.x % Utilities::CHUNK_WIDTH < 0)
		{
			position.x += std::abs(position.x % Utilities::CHUNK_WIDTH);
			position.x -= Utilities::CHUNK_WIDTH;
		}
		else if (position.x % Utilities::CHUNK_WIDTH > 0)
		{
			position.x -= std::abs(position.x % Utilities::CHUNK_WIDTH);
		}
		if (position.z % Utilities::CHUNK_DEPTH < 0)
		{
			position.z += std::abs(position.z % Utilities::CHUNK_DEPTH);
			position.z -= Utilities::CHUNK_DEPTH;
		}
		else if (position.z % Utilities::CHUNK_DEPTH > 0)
		{
			position.z -= std::abs(position.z % Utilities::CHUNK_DEPTH);
		}
	}

	glm::ivec3 getNeighbouringChunkPosition(const glm::ivec3& chunkStartingPosition, eDirection direction)
	{
		switch (direction)
		{
		case eDirection::Left:
			return glm::ivec3(chunkStartingPosition.x - Utilities::CHUNK_WIDTH, 0, chunkStartingPosition.z);

		case eDirection::Right:
			return glm::ivec3(chunkStartingPosition.x + Utilities::CHUNK_WIDTH, 0, chunkStartingPosition.z);

		case eDirection::Forward:
			return glm::ivec3(chunkStartingPosition.x, 0, chunkStartingPosition.z + Utilities::CHUNK_DEPTH);

		case eDirection::Back:
			return glm::ivec3(chunkStartingPosition.x, 0, chunkStartingPosition.z - Utilities::CHUNK_DEPTH);

		default:
			assert(false);
		}
	}

	float getSqrMagnitude(const glm::ivec3& positionA, const glm::ivec3& positionB)
	{
		return glm::pow(positionA.x - positionB.x, 2) +
			glm::pow(positionA.y - positionB.y, 2) +
			glm::pow(positionA.z - positionB.z, 2);
	}
}

//NeighbouringChunks
NeighbouringChunks::NeighbouringChunks(const Chunk& leftChunk, const Chunk& rightChunk, const Chunk& topChunk, const Chunk& bottomChunk)
	: leftChunk(leftChunk),
	rightChunk(rightChunk),
	forwardChunk(topChunk),
	backChunk(bottomChunk)
{}

//GeneratedChunkMesh
GeneratedChunkMesh::GeneratedChunkMesh(const glm::ivec3& position, ObjectFromPool<VertexArray>&& chunkMeshFromPool)
	: ObjectQueueNode(position),
	chunkMeshFromPool(std::move(chunkMeshFromPool))
{}

GeneratedChunkMesh::GeneratedChunkMesh(GeneratedChunkMesh&& orig) noexcept
	: ObjectQueueNode(std::move(orig)),
	chunkMeshFromPool(std::move(orig.chunkMeshFromPool))
{}

GeneratedChunkMesh& GeneratedChunkMesh::operator=(GeneratedChunkMesh&& orig) noexcept
{
	ObjectQueueNode::operator=(std::move(orig));
	chunkMeshFromPool = std::move(orig.chunkMeshFromPool);
	
	return *this;
}

//GeneratedChunk
GeneratedChunk::GeneratedChunk(const glm::ivec3& position, ObjectFromPool<Chunk>&& chunkFromPool)
	: ObjectQueueNode(position),
	chunkFromPool(std::move(chunkFromPool))
{}

GeneratedChunk::GeneratedChunk(GeneratedChunk&& orig) noexcept
	: ObjectQueueNode(std::move(orig)),
	chunkFromPool(std::move(orig.chunkFromPool))
{}

GeneratedChunk& GeneratedChunk::operator=(GeneratedChunk&& orig) noexcept
{
	ObjectQueueNode::operator=(std::move(orig));
	chunkFromPool = std::move(orig.chunkFromPool);

	return *this;
}

//ChunkGenerator
ChunkManager::ChunkManager(const glm::ivec3& playerPosition)
	: m_chunkPool(getObjectPoolSize()),
	m_chunkMeshPool(getObjectPoolSize()),
	m_chunks(),
	m_chunkMeshes(),
	m_chunkMeshesToGenerateQueue(),
	m_deletionQueue(),
	m_generatedChunkMeshesQueue(),
	m_generatedChunkQueue()
{
	addChunks(playerPosition);
}

void ChunkManager::update(const Player& player, const sf::Window& window, std::atomic<bool>& resetGame, 
	std::mutex& playerMutex, std::mutex& renderingMutex)	
{
	while (!resetGame && window.isOpen())
	{
		std::unique_lock<std::mutex> playerLock(playerMutex);
		glm::ivec3 playerPosition = player.getPosition();
		playerLock.unlock();

		deleteChunks(playerPosition, renderingMutex);
		addChunks(playerPosition);
		generateChunkMeshes();
		clearQueues(playerPosition);

		std::lock_guard<std::mutex> renderingLock(renderingMutex); 
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

			if (!m_generatedChunkQueue.isEmpty())
			{
				GeneratedChunk& generatedChunk = m_generatedChunkQueue.front();
				assert(generatedChunk.chunkFromPool.getObject());

				m_chunks.emplace(std::piecewise_construct,
					std::forward_as_tuple(generatedChunk.getPosition()),
					std::forward_as_tuple(std::move(generatedChunk.chunkFromPool)));

				m_generatedChunkQueue.pop();
			}

			if (!m_generatedChunkMeshesQueue.isEmpty())
			{
				GeneratedChunkMesh& generatedChunkMesh = m_generatedChunkMeshesQueue.front();
				assert(generatedChunkMesh.chunkMeshFromPool.getObject());

				m_chunkMeshes.emplace(std::piecewise_construct,
					std::forward_as_tuple(generatedChunkMesh.getPosition()),
					std::forward_as_tuple(std::move(generatedChunkMesh.chunkMeshFromPool)));

				m_generatedChunkMeshesQueue.pop();
			}
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

void ChunkManager::deleteChunks(const glm::ivec3& playerPosition, std::mutex& renderingMutex)
{
	glm::ivec3 startingPosition(playerPosition);
	getClosestMiddlePosition(startingPosition);
	Rectangle visibilityRect(glm::vec2(startingPosition.x, startingPosition.z), Utilities::VISIBILITY_DISTANCE);

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
	glm::ivec3 startPosition(playerPosition);
	getClosestMiddlePosition(startPosition);
	for (int z = startPosition.z - Utilities::VISIBILITY_DISTANCE; z <= startPosition.z + Utilities::VISIBILITY_DISTANCE; z += Utilities::CHUNK_DEPTH)
	{
		for (int x = startPosition.x - Utilities::VISIBILITY_DISTANCE; x <= startPosition.x + Utilities::VISIBILITY_DISTANCE; x += Utilities::CHUNK_WIDTH)
		{
			glm::ivec3 chunkStartingPosition(x, 0, z);
			getClosestChunkStartingPosition(chunkStartingPosition);

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

void ChunkManager::generateChunkMeshes()
{
	if (m_chunkMeshesToGenerateQueue.isEmpty())
	{
		return;
	}

	PositionNode* chunkMeshToGenerate = &m_chunkMeshesToGenerateQueue.front();
	while (chunkMeshToGenerate)
	{
		const glm::ivec3& chunkStartingPosition = chunkMeshToGenerate->getPosition();
		auto leftChunk = m_chunks.find(getNeighbouringChunkPosition(chunkStartingPosition, eDirection::Left));
		auto rightChunk = m_chunks.find(getNeighbouringChunkPosition(chunkStartingPosition, eDirection::Right));
		auto forwardChunk = m_chunks.find(getNeighbouringChunkPosition(chunkStartingPosition, eDirection::Forward));
		auto backChunk = m_chunks.find(getNeighbouringChunkPosition(chunkStartingPosition, eDirection::Back));
		auto chunk = m_chunks.find(chunkStartingPosition);

		if (leftChunk != m_chunks.cend() &&
			rightChunk != m_chunks.cend() &&
			forwardChunk != m_chunks.cend() &&
			backChunk != m_chunks.cend() && 
			chunk != m_chunks.cend())
		{
			ObjectFromPool<VertexArray> chunkMeshFromPool = m_chunkMeshPool.getNextAvailableObject();
			if (chunkMeshFromPool.getObject())
			{
				generateChunkMesh(*chunkMeshFromPool.getObject(), *chunk->second.getObject(),
					{ *leftChunk->second.getObject(), *rightChunk->second.getObject(), *forwardChunk->second.getObject(), *backChunk->second.getObject() });

				m_generatedChunkMeshesQueue.add({ chunkStartingPosition, std::move(chunkMeshFromPool) });
				chunkMeshToGenerate = m_chunkMeshesToGenerateQueue.remove(chunkMeshToGenerate);
			}
			else
			{
				chunkMeshToGenerate = m_chunkMeshesToGenerateQueue.next(chunkMeshToGenerate);
			}
		}
		else
		{
			chunkMeshToGenerate = m_chunkMeshesToGenerateQueue.next(chunkMeshToGenerate);
		}
	}
}

void ChunkManager::clearQueues(const glm::ivec3& playerPosition)
{
	//Clears queues that are out of bounds of Player AABB
	glm::ivec3 startingPosition(playerPosition);
	getClosestMiddlePosition(startingPosition);
	Rectangle visibilityRect(glm::vec2(startingPosition.x, startingPosition.z), Utilities::VISIBILITY_DISTANCE);

	if (!m_chunkMeshesToGenerateQueue.isEmpty())
	{
		PositionNode* chunkMeshToGenerate = &m_chunkMeshesToGenerateQueue.front();
		while (chunkMeshToGenerate)
		{
			glm::ivec2 centrePosition(chunkMeshToGenerate->getPosition().x + 16, chunkMeshToGenerate->getPosition().z + 16);
			Rectangle AABB(centrePosition, 16);
			if (!visibilityRect.contains(AABB))
			{
				chunkMeshToGenerate = m_chunkMeshesToGenerateQueue.remove(chunkMeshToGenerate);
			}
			else
			{
				chunkMeshToGenerate = m_chunkMeshesToGenerateQueue.next(chunkMeshToGenerate);
			}
		}
	}

	if (!m_generatedChunkMeshesQueue.isEmpty())
	{
		GeneratedChunkMesh* generatedChunkMesh = &m_generatedChunkMeshesQueue.front();
		while (generatedChunkMesh)
		{
			glm::ivec2 centrePosition(generatedChunkMesh->getPosition().x + 16, generatedChunkMesh->getPosition().z + 16);
			Rectangle AABB(centrePosition, 16);
			if (!visibilityRect.contains(AABB))
			{
				generatedChunkMesh = m_generatedChunkMeshesQueue.remove(generatedChunkMesh);
			}
			else
			{
				generatedChunkMesh = m_generatedChunkMeshesQueue.next(generatedChunkMesh);
			}
		}
	}

	if (!m_generatedChunkQueue.isEmpty())
	{
		GeneratedChunk* generatedChunkNode = &m_generatedChunkQueue.front();
		while (generatedChunkNode)
		{
			glm::ivec2 centrePosition(generatedChunkNode->getPosition().x + 16, generatedChunkNode->getPosition().z + 16);
			Rectangle AABB(centrePosition, 16);
			if (!visibilityRect.contains(AABB))
			{
				generatedChunkNode = m_generatedChunkQueue.remove(generatedChunkNode);
			}
			else
			{
				generatedChunkNode = m_generatedChunkQueue.next(generatedChunkNode);
			}
		}
	}
}