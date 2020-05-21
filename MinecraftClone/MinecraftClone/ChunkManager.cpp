#include "ChunkManager.h"
#include "Utilities.h"
#include "VertexBuffer.h"
#include "VertexArray.h"
#include "CubeType.h"
#include "Rectangle.h"
#include "Camera.h"
#include "Frustum.h"
#include "ChunkMeshGenerator.h"
#include <iostream>
#include <deque>

namespace
{
	constexpr int THREAD_TRANSFER_PER_FRAME = 1;

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

//ChunkMeshToGenerate
GeneratedChunkMesh::GeneratedChunkMesh(ObjectFromPool<VertexArray>&& vertexArrayFromPool, const glm::ivec3& chunkStartingPosition)
	: ObjectQueueNode(chunkStartingPosition),
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
	: m_chunkPool(Utilities::VISIBILITY_DISTANCE, Utilities::CHUNK_WIDTH, Utilities::CHUNK_DEPTH),
	m_vertexArrayPool(Utilities::VISIBILITY_DISTANCE, Utilities::CHUNK_WIDTH, Utilities::CHUNK_DEPTH),
	m_chunks(),
	m_VAOs(),
	m_generatedChunkMeshes(),
	m_deletedChunks()
{
	addChunks(playerPosition);
}

void ChunkManager::update(const glm::vec3& cameraPosition, const sf::Window& window, std::atomic<bool>& resetGame, 
	std::mutex& cameraMutex, std::mutex& renderingMutex)	
{
	while (!resetGame && window.isOpen())
	{
		std::unique_lock<std::mutex> cameraLock(cameraMutex);
		glm::ivec3 position = cameraPosition;
		cameraLock.unlock();

		deleteChunks(position, renderingMutex);
		addChunks(position);
		generateChunkMeshes();

		std::lock_guard<std::mutex> renderingLock(renderingMutex);
		for (int i = 0; i < THREAD_TRANSFER_PER_FRAME; ++i)
		{
			if (!m_deletedChunks.isEmpty())
			{
				const glm::ivec3& chunkStartingPosition = m_deletedChunks.front().position;
				auto VAO = m_VAOs.find(chunkStartingPosition);
				//assert(VAO != m_VAOs.end());
				if (VAO != m_VAOs.end())
				{
					m_VAOs.erase(VAO);
				}

				m_deletedChunks.pop();
			}

			if (!m_generatedChunkMeshes.isEmpty())
			{
				GeneratedChunkMesh& generatedChunkMesh = m_generatedChunkMeshes.front();

				m_VAOs.emplace(std::piecewise_construct,
					std::forward_as_tuple(generatedChunkMesh.position),
					std::forward_as_tuple(std::move(generatedChunkMesh.vertexArrayFromPool)));

				m_generatedChunkMeshes.pop();
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
		if (!visibilityRect.contains(chunk->second.getObject()->getAABB()))
		{
			//auto iter = std::find_if(m_chunkMeshesToGenerate.begin(), m_chunkMeshesToGenerate.end(), [chunkStartingPosition](const auto& chunkMeshToGenerate)
			//{
			//	return chunkMeshToGenerate.startingPosition == chunkStartingPosition;
			//});
			//if (iter != m_chunkMeshesToGenerate.end())
			//{
			//	m_chunkMeshesToGenerate.erase(iter);
			//}

			if(!m_deletedChunks.contains(chunkStartingPosition))
			{
				m_deletedChunks.add({ chunkStartingPosition });
			}

			m_generatedChunkMeshes.remove(chunkStartingPosition);

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
	std::vector<ChunkToAdd> addedChunks;
	glm::ivec3 startPosition(playerPosition);
	getClosestMiddlePosition(startPosition);
	for (int z = startPosition.z - Utilities::VISIBILITY_DISTANCE; z <= startPosition.z + Utilities::VISIBILITY_DISTANCE; z += Utilities::CHUNK_DEPTH)
	{
		for (int x = startPosition.x - Utilities::VISIBILITY_DISTANCE; x <= startPosition.x + Utilities::VISIBILITY_DISTANCE; x += Utilities::CHUNK_WIDTH)
		{
			glm::ivec3 chunkStartingPosition(x, 0, z);
			getClosestChunkStartingPosition(chunkStartingPosition);

			if (m_chunks.find(chunkStartingPosition) == m_chunks.cend() && m_VAOs.find(chunkStartingPosition) == m_VAOs.cend())
			{
				ObjectFromPool<Chunk> chunkFromPool = m_chunkPool.getNextAvailableObject();
				if (!chunkFromPool.getObject())
				{
					std::cout << "ran out of chunks\n";
					continue;
				}

				ObjectFromPool<Chunk>& addedChunk = m_chunks.emplace(std::piecewise_construct,
					std::forward_as_tuple(chunkStartingPosition),
					std::forward_as_tuple(std::move(chunkFromPool))).first->second;

				addedChunk.getObject()->reuse(chunkStartingPosition);

				//std::cout << chunkStartingPosition.x << " " << chunkStartingPosition.y << " " << chunkStartingPosition.z << "\n";
				
				addedChunks.emplace_back(getSqrMagnitude(chunkStartingPosition, playerPosition), chunkStartingPosition, *addedChunk.getObject());
				
				m_deletedChunks.remove(chunkStartingPosition);
			}
		}
	}

	if (!addedChunks.empty())
	{
		std::sort(addedChunks.begin(), addedChunks.end(), [](const auto& a, const auto& b)
		{
			return a.distanceFromCamera < b.distanceFromCamera;
		});

		for (const auto& addedChunk : addedChunks)
		{
			auto chunk = m_chunks.find(addedChunk.startingPosition);
			assert(chunk != m_chunks.cend());

			auto leftChunk = m_chunks.find(getNeighbouringChunkPosition(chunk->first, eDirection::Left));
			auto rightChunk = m_chunks.find(getNeighbouringChunkPosition(chunk->first, eDirection::Right));
			auto forwardChunk = m_chunks.find(getNeighbouringChunkPosition(chunk->first, eDirection::Forward));
			auto backChunk = m_chunks.find(getNeighbouringChunkPosition(chunk->first, eDirection::Back));

			if (leftChunk != m_chunks.cend() &&
				rightChunk != m_chunks.cend() &&
				forwardChunk != m_chunks.cend() &&
				backChunk != m_chunks.cend())
			{
				ObjectFromPool<VertexArray> vertexArrayFromPool = m_vertexArrayPool.getNextAvailableObject();
				if (!vertexArrayFromPool.getObject())
				{
					std::cout << " Ran out of VAO's\n";
					continue;
				}

				generateChunkMesh(*vertexArrayFromPool.getObject(), addedChunk.chunk,
					{ *leftChunk->second.getObject(), *rightChunk->second.getObject(), *forwardChunk->second.getObject(), *backChunk->second.getObject() });

				m_generatedChunkMeshes.add({ std::move(vertexArrayFromPool), addedChunk.startingPosition });
			}
			else
			{
				m_chunkMeshesToGenerate.push_back(addedChunk);
			}
		}

		std::sort(m_chunkMeshesToGenerate.begin(), m_chunkMeshesToGenerate.end(), [](const auto& a, const auto& b)
		{
			return a.distanceFromCamera < b.distanceFromCamera;
		});
	}



	//while (!addedChunks.empty())
	//{
	//	const ChunkToAdd& addedChunk = addedChunks.front();

	//	ObjectFromPool<VertexArray> vertexArrayFromPool = m_vertexArrayPool.getNextAvailableObject();
	//	if (!vertexArrayFromPool.getObject())
	//	{
	//		continue;
	//	}

	//	m_chunksToDelete.remove(addedChunk.startingPosition);
	//	m_generatedChunkMeshes.remove(addedChunk.startingPosition);

	//	if (isChunkNeighboured(addedChunk.startingPosition))
	//	{
	//		generateChunkMesh(*vertexArrayFromPool.getObject(), addedChunk.chunk, )

	//		m_chunkMeshesToGenerate.emplace(std::piecewise_construct,
	//			std::forward_as_tuple(chunkToAdd.startingPosition),
	//			std::forward_as_tuple(chunkToAdd.startingPosition, std::move(vertexArrayFromPool)));
	//	}

	//	addedChunks.pop_front();
	//}
}

void ChunkManager::generateChunkMeshes()
{
	for (auto chunkMeshToGenerate = m_chunkMeshesToGenerate.begin(); chunkMeshToGenerate != m_chunkMeshesToGenerate.end();)
	{
		//If Chunk has no neighbours - then it can be Generated
		auto leftChunk = m_chunks.find(getNeighbouringChunkPosition(chunkMeshToGenerate->startingPosition, eDirection::Left));
		auto rightChunk = m_chunks.find(getNeighbouringChunkPosition(chunkMeshToGenerate->startingPosition, eDirection::Right));
		auto forwardChunk = m_chunks.find(getNeighbouringChunkPosition(chunkMeshToGenerate->startingPosition, eDirection::Forward));
		auto backChunk = m_chunks.find(getNeighbouringChunkPosition(chunkMeshToGenerate->startingPosition, eDirection::Back));

		if (leftChunk != m_chunks.cend() &&
			rightChunk != m_chunks.cend() &&
			forwardChunk != m_chunks.cend() &&
			backChunk != m_chunks.cend())
		{
			ObjectFromPool<VertexArray> vertexArrayFromPool = m_vertexArrayPool.getNextAvailableObject();
			if (!vertexArrayFromPool.getObject())
			{
				std::cout << "Ran out of VAO's\n";
				continue;
			}

			generateChunkMesh(*vertexArrayFromPool.getObject(), chunkMeshToGenerate->chunk,
				{ *leftChunk->second.getObject(), *rightChunk->second.getObject(), *forwardChunk->second.getObject(), *backChunk->second.getObject() });

			m_generatedChunkMeshes.add({ std::move(vertexArrayFromPool), chunkMeshToGenerate->startingPosition });

			//if (!m_generatedChunkMeshes.contains(chunkMeshToGenerate->first))
			//{
			//	m_generatedChunkMeshes.remove(chunkMeshToGenerate->first);
			//	assert(!m_chunksToDelete.contains(chunkMeshToGenerate->first));

			//	generateChunkMesh(chunkMeshToGenerate->second,
			//		{ *leftChunk->second.getObject(), *rightChunk->second.getObject(), *forwardChunk->second.getObject(), *backChunk->second.getObject() });
			//	
			//	m_generatedChunkMeshes.add(chunkMeshToGenerate->first);
			//}
			chunkMeshToGenerate = m_chunkMeshesToGenerate.erase(chunkMeshToGenerate);
		}
		else
		{
			++chunkMeshToGenerate;
		}
	}
}