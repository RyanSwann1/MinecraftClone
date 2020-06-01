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
GeneratedChunkMesh::GeneratedChunkMesh(const glm::ivec3& position, ObjectFromPool<VertexArray>&& vertexArrayFromPool)
	: ObjectQueueNode(position),
	vertexArrayFromPool(std::move(vertexArrayFromPool))
{}

GeneratedChunkMesh::GeneratedChunkMesh(GeneratedChunkMesh&& orig) noexcept
	: ObjectQueueNode(std::move(orig)),
	vertexArrayFromPool(std::move(orig.vertexArrayFromPool))
{}

GeneratedChunkMesh& GeneratedChunkMesh::operator=(GeneratedChunkMesh&& orig) noexcept
{
	ObjectQueueNode::operator=(std::move(orig));
	vertexArrayFromPool = std::move(orig.vertexArrayFromPool);
	
	return *this;
}

//ChunkGenerator
ChunkManager::ChunkManager(const glm::ivec3& playerPosition)
	: m_chunkPool(getObjectPoolSize()),
	m_vertexArrayPool(getObjectPoolSize()),
	m_chunks(),
	m_VAOs(),
	m_chunkMeshesToGenerateQueue(),
	m_deletedChunksQueue(),
	m_generatedChunkMeshesQueue()
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

		//playerLock.lock(); 
		std::lock_guard<std::mutex> renderingLock(renderingMutex); 
		for (int i = 0; i < THREAD_TRANSFER_PER_FRAME; ++i)
		{
			if (!m_deletedChunksQueue.isEmpty())
			{
				const glm::ivec3& chunkStartingPosition = m_deletedChunksQueue.front().getPosition();
				auto VAO = m_VAOs.find(chunkStartingPosition);
				if (VAO != m_VAOs.end())
				{
					m_VAOs.erase(VAO);
				}

				m_deletedChunksQueue.pop();
			}

			if (!m_generatedChunkMeshesQueue.isEmpty())
			{
				GeneratedChunkMesh& generatedChunkMesh = m_generatedChunkMeshesQueue.front();
				assert(generatedChunkMesh.vertexArrayFromPool.getObject());

				m_VAOs.emplace(std::piecewise_construct,
					std::forward_as_tuple(generatedChunkMesh.getPosition()),
					std::forward_as_tuple(std::move(generatedChunkMesh.vertexArrayFromPool)));

				m_generatedChunkMeshesQueue.pop();
			}
		}
	}
}

void ChunkManager::renderOpaque(const Frustum& frustum) const
{
	for (const auto& VAO : m_VAOs)
	{
		if (VAO.second.getObject()->m_opaqueVertexBuffer.bindToVAO)
		{
			VAO.second.getObject()->attachOpaqueVBO();
		}

		if (VAO.second.getObject()->m_opaqueVertexBuffer.displayable && frustum.isChunkInFustrum(VAO.first))
		{
			VAO.second.getObject()->bindOpaqueVAO();
			glDrawElements(GL_TRIANGLES, VAO.second.getObject()->m_opaqueVertexBuffer.indicies.size(), GL_UNSIGNED_INT, nullptr);
		}
	}
}

void ChunkManager::renderTransparent(const Frustum& frustum) const
{
	for (const auto& VAO : m_VAOs)
	{
		if (VAO.second.getObject()->m_transparentVertexBuffer.bindToVAO)
		{
			VAO.second.getObject()->attachTransparentVBO();
		}

		if (VAO.second.getObject()->m_transparentVertexBuffer.displayable && frustum.isChunkInFustrum(VAO.first))
		{
			VAO.second.getObject()->bindTransparentVAO();
			glDrawElements(GL_TRIANGLES, VAO.second.getObject()->m_transparentVertexBuffer.indicies.size(), GL_UNSIGNED_INT, nullptr);
		}
	}
}

void ChunkManager::deleteChunks(const glm::ivec3& playerPosition, std::mutex& renderingMutex)
{
	glm::ivec3 startingPosition(playerPosition);
	getClosestMiddlePosition(startingPosition);
	Rectangle visibilityRect(glm::vec2(startingPosition.x, startingPosition.z), Utilities::VISIBILITY_DISTANCE);

	for (auto chunk = m_chunks.begin(); chunk != m_chunks.end();)
	{
		const glm::ivec3& chunkStartingPosition = chunk->second.getObject()->getStartingPosition();
		if (!m_deletedChunksQueue.contains(chunkStartingPosition) &&
			!visibilityRect.contains(chunk->second.getObject()->getAABB()))
		{
			m_deletedChunksQueue.add({ chunkStartingPosition });
			chunk = m_chunks.erase(chunk);
		}
		else
		{
			++chunk;
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
				m_VAOs.find(chunkStartingPosition) == m_VAOs.cend() && 
				!m_chunkMeshesToGenerateQueue.contains(chunkStartingPosition) &&
				!m_generatedChunkMeshesQueue.contains(chunkStartingPosition))
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
				ObjectFromPool<Chunk>& addedChunk = m_chunks.emplace(std::piecewise_construct,
					std::forward_as_tuple(chunkToAdd.startingPosition),
					std::forward_as_tuple(std::move(chunkFromPool))).first->second;

				addedChunk.getObject()->reuse(chunkToAdd.startingPosition);

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
			
		if (leftChunk != m_chunks.cend() &&
			rightChunk != m_chunks.cend() &&
			forwardChunk != m_chunks.cend() &&
			backChunk != m_chunks.cend())
		{
			ObjectFromPool<VertexArray> vertexArrayFromPool = m_vertexArrayPool.getNextAvailableObject();
			if (vertexArrayFromPool.getObject())
			{
				auto chunk = m_chunks.find(chunkStartingPosition);
				assert(chunk != m_chunks.end());

				generateChunkMesh(*vertexArrayFromPool.getObject(), *chunk->second.getObject(),
					{ *leftChunk->second.getObject(), *rightChunk->second.getObject(), *forwardChunk->second.getObject(), *backChunk->second.getObject() });

				m_generatedChunkMeshesQueue.add({ chunkStartingPosition, std::move(vertexArrayFromPool) });
				chunkMeshToGenerate = m_chunkMeshesToGenerateQueue.remove(chunkStartingPosition);
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
				chunkMeshToGenerate = m_chunkMeshesToGenerateQueue.remove(chunkMeshToGenerate->getPosition());
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
				generatedChunkMesh = m_generatedChunkMeshesQueue.remove(generatedChunkMesh->getPosition());
			}
			else
			{
				generatedChunkMesh = m_generatedChunkMeshesQueue.next(generatedChunkMesh);
			}
		}
	}


}