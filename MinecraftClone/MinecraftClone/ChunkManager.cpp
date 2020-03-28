#include "ChunkManager.h"
#include "Utilities.h"
#include "VertexBuffer.h"
#include "VertexArray.h"
#include "CubeID.h"
#include "Rectangle.h"
#include "Camera.h"
#include <iostream>

ChunkManager::ChunkManager()
	: m_chunkPool(),
	m_vertexArrayPool(),
	m_VAOs(),
	m_chunks()
{}

void ChunkManager::generateInitialChunks(const glm::vec3& playerPosition)
{
	std::vector<std::pair<const Chunk*, VertexArray*>> recentlyAdded;
	for (int z = playerPosition.z - Utilities::VISIBILITY_DISTANCE; z <= playerPosition.z + Utilities::VISIBILITY_DISTANCE; z += Utilities::CHUNK_DEPTH)
	{
		for (int x = playerPosition.x - Utilities::VISIBILITY_DISTANCE; x <= playerPosition.x + Utilities::VISIBILITY_DISTANCE; x += Utilities::CHUNK_WIDTH)
		{
			glm::ivec3 chunkStartingPosition(x, 0, z);
			Utilities::getClosestChunkStartingPosition(chunkStartingPosition);
			assert(m_chunks.find(chunkStartingPosition) == m_chunks.cend() && m_VAOs.find(chunkStartingPosition) == m_VAOs.cend());
			
			VertexArray& VAO = *m_VAOs.emplace(std::piecewise_construct,
				std::forward_as_tuple(chunkStartingPosition),
				std::forward_as_tuple(m_vertexArrayPool)).first->second.object;

			const Chunk& chunk = *m_chunks.emplace(std::piecewise_construct,
				std::forward_as_tuple(chunkStartingPosition),
				std::forward_as_tuple(m_chunkPool, chunkStartingPosition)).first->second.object;
		
			recentlyAdded.emplace_back(&chunk, &VAO);
		}
	}

	for (const auto& added : recentlyAdded)
	{
		generateChunkMesh(*added.second, *added.first);
	}

	for (auto VAO = m_VAOs.begin(); VAO != m_VAOs.end();)
	{
		if (VAO->second.object->m_awaitingRegeneration)
		{
			const glm::ivec3& chunkStartingPosition = VAO->first;

			auto chunk = m_chunks.find(chunkStartingPosition);
			assert(chunk != m_chunks.cend());
			if (chunk != m_chunks.cend())
			{
				m_regenerate.emplace(std::piecewise_construct,
					std::forward_as_tuple(chunkStartingPosition),
					std::forward_as_tuple(*chunk->second.object, std::move(VAO->second)));
			}

			VAO = m_VAOs.erase(VAO);
		}
		else
		{
			++VAO;
		}
	}
}

void ChunkManager::update(const glm::vec3& cameraPosition, const sf::Window& window, std::atomic<bool>& resetGame, 
	std::mutex& cameraMutex, std::mutex& renderingMutex)	
{
	while (!resetGame)
	{
		std::unique_lock<std::mutex> lock(cameraMutex);
		glm::ivec3 position = cameraPosition;
		lock.unlock();

		deleteChunks(position, renderingMutex);
		addChunks(position, renderingMutex);
		handleRegeneration(renderingMutex);

		std::this_thread::sleep_for(std::chrono::milliseconds(400));
	}
}

void ChunkManager::renderOpaque() const
{
	for (auto& VAO : m_VAOs)
	{
		if (VAO.second.object->m_attachOpaqueVBO)
		{
			VAO.second.object->attachOpaqueVBO();
		}

		if (VAO.second.object->m_attachTransparentVBO)
		{
			VAO.second.object->attachTransparentVBO();
		}

		if (VAO.second.object->m_opaqueVBODisplayable)
		{
			VAO.second.object->bindOpaqueVAO();
			glDrawElements(GL_TRIANGLES, VAO.second.object->m_vertexBuffer.indicies.size(), GL_UNSIGNED_INT, nullptr);
		}
	}
}

void ChunkManager::renderTransparent() const
{
	for (const auto& VAO : m_VAOs)
	{
		if (VAO.second.object->m_transparentVBODisplayable)
		{
			VAO.second.object->bindTransparentVAO();
			glDrawElements(GL_TRIANGLES, VAO.second.object->m_vertexBuffer.transparentIndicies.size(), GL_UNSIGNED_INT, nullptr);
		}
	}
}

void ChunkManager::addCubeFace(VertexArray& vertexArray, eCubeType cubeType, eCubeSide cubeSide, const glm::ivec3& cubePosition)
{
	glm::ivec3 position = cubePosition;
	if (cubeType == eCubeType::Water)
	{
		assert(cubeSide == eCubeSide::Top);
		if (cubeSide == eCubeSide::Top)
		{
			for (const glm::vec3& i : Utilities::CUBE_FACE_TOP)
			{
				position += i;
				vertexArray.m_vertexBuffer.transparentPositions.push_back({ position.x, position.y, position.z });
				position = cubePosition;
			}
			
			Utilities::getTextCoords(vertexArray.m_vertexBuffer.transparentTextCoords, cubeSide, cubeType);
			
			for (unsigned int i : Utilities::CUBE_FACE_INDICIES)
			{
				vertexArray.m_vertexBuffer.transparentIndicies.push_back(i + vertexArray.m_transparentElementBufferIndex);
			}

			vertexArray.m_transparentElementBufferIndex += Utilities::CUBE_FACE_INDICIE_COUNT;
		}
	}
	else
	{
		switch (cubeSide)
		{
		case eCubeSide::Front:
			for (const glm::ivec3& i : Utilities::CUBE_FACE_FRONT)
			{
				position += i;
				vertexArray.m_vertexBuffer.positions.emplace_back(position.x, position.y, position.z);
				position = cubePosition;
			}
			
			Utilities::getTextCoords(vertexArray.m_vertexBuffer.textCoords, cubeSide, cubeType);
			break;
		case eCubeSide::Back:
			for (glm::ivec3 i : Utilities::CUBE_FACE_BACK)
			{
				position += i;
				vertexArray.m_vertexBuffer.positions.emplace_back(position.x, position.y, position.z);
				position = cubePosition;
			}

			Utilities::getTextCoords(vertexArray.m_vertexBuffer.textCoords, cubeSide, cubeType);

			break;
		case eCubeSide::Left:
			for (const glm::ivec3& i : Utilities::CUBE_FACE_LEFT)
			{
				position += i;
				vertexArray.m_vertexBuffer.positions.emplace_back(position.x, position.y, position.z);
				position = cubePosition;
			}
			Utilities::getTextCoords(vertexArray.m_vertexBuffer.textCoords, cubeSide, cubeType);
			break;
		case eCubeSide::Right:
			for (const glm::ivec3& i : Utilities::CUBE_FACE_RIGHT)
			{
				position += i;
				vertexArray.m_vertexBuffer.positions.emplace_back(position.x, position.y, position.z);
				position = cubePosition;
			}

			Utilities::getTextCoords(vertexArray.m_vertexBuffer.textCoords, cubeSide, cubeType);

			break;
		case eCubeSide::Top:
			for (const glm::ivec3& i : Utilities::CUBE_FACE_TOP)
			{
				position += i;
				vertexArray.m_vertexBuffer.positions.emplace_back(position.x, position.y, position.z);
				position = cubePosition;
			}
			Utilities::getTextCoords(vertexArray.m_vertexBuffer.textCoords, cubeSide, cubeType);
			
			break;
		case eCubeSide::Bottom:
			for (const glm::ivec3& i : Utilities::CUBE_FACE_BOTTOM)
			{
				position += i;
				vertexArray.m_vertexBuffer.positions.emplace_back(position.x, position.y, position.z);
				position = cubePosition;
			}	

			Utilities::getTextCoords(vertexArray.m_vertexBuffer.textCoords, cubeSide, cubeType);

			break;
		}

		for (unsigned int i : Utilities::CUBE_FACE_INDICIES)
		{
			vertexArray.m_vertexBuffer.indicies.push_back(i + vertexArray.m_opaqueElementBufferIndex);
		}

		vertexArray.m_opaqueElementBufferIndex += Utilities::CUBE_FACE_INDICIE_COUNT;
	}
}

bool ChunkManager::isCubeAtPosition(const glm::ivec3& position, const Chunk& chunk) const
{
	char cubeType = chunk.getCubeDetailsWithoutBoundsCheck(position);
	return (cubeType != static_cast<char>(eCubeType::Invalid) && cubeType != static_cast<char>(eCubeType::Water) ? true : false);
}

void ChunkManager::generateChunkMesh(VertexArray& vertexArray, const Chunk& chunk)
{
	const glm::ivec3& chunkStartingPosition = chunk.getStartingPosition();
	const glm::ivec3& chunkEndingPosition = chunk.getEndingPosition();
	bool regenChunk = false;

	const Chunk* leftNeighbouringChunk = getNeighbouringChunkAtPosition(chunkStartingPosition, eDirection::Left);
	const Chunk* rightNeighbouringChunk = getNeighbouringChunkAtPosition(chunkStartingPosition, eDirection::Right);
	const Chunk* forwardNeighbouringChunk = getNeighbouringChunkAtPosition(chunkStartingPosition, eDirection::Forward);
	const Chunk* backNeighbouringChunk = getNeighbouringChunkAtPosition(chunkStartingPosition, eDirection::Back);

	for (int z = chunkStartingPosition.z; z < chunkEndingPosition.z; ++z)
	{
		for (int y = chunkStartingPosition.y; y < chunkEndingPosition.y; ++y)
		{
			for (int x = chunkStartingPosition.x; x < chunkEndingPosition.x; ++x)
			{
				glm::ivec3 position(x, y, z);
				eCubeType cubeType = static_cast<eCubeType>(chunk.getCubeDetailsWithoutBoundsCheck(position));
				if (cubeType == eCubeType::Invalid)
				{
					continue;
				}
				else if (cubeType == eCubeType::Water)
				{
					if (!vertexArray.m_awaitingRegeneration)
					{
						addCubeFace(vertexArray, cubeType, eCubeSide::Top, position);
					}
				}
				else
				{
					//Left Face
					glm::ivec3 leftPosition(x - 1, y, z);
					if (chunk.isPositionInBounds(leftPosition))
					{
						if (!isCubeAtPosition(leftPosition, chunk))
						{
							addCubeFace(vertexArray, cubeType, eCubeSide::Left, position);
						}
					}
					else if (leftNeighbouringChunk &&
						!isCubeAtPosition(leftPosition, *leftNeighbouringChunk))
					{
						addCubeFace(vertexArray, cubeType, eCubeSide::Left, position);
					}
					else if (!leftNeighbouringChunk)
					{
						regenChunk = true;
					}
					
					//Right Face
					glm::ivec3 rightPosition(x + 1, y, z);
					if (chunk.isPositionInBounds(rightPosition))
					{
						if (!isCubeAtPosition(rightPosition, chunk))
						{
							addCubeFace(vertexArray, cubeType, eCubeSide::Right, position);
						}
					}
					else if (rightNeighbouringChunk &&
						!isCubeAtPosition(rightPosition, *rightNeighbouringChunk))
					{
						addCubeFace(vertexArray, cubeType, eCubeSide::Right, position);
					}
					else if (!rightNeighbouringChunk)
					{
						regenChunk = true;
					}

					//Forward Face
					glm::ivec3 forwardPosition(x, y, z + 1);
					if (chunk.isPositionInBounds(forwardPosition))
					{
						if (!isCubeAtPosition(forwardPosition, chunk))
						{
							addCubeFace(vertexArray, cubeType, eCubeSide::Front, position);
						}
					}
					else if (forwardNeighbouringChunk &&
						!isCubeAtPosition(forwardPosition, *forwardNeighbouringChunk))
					{
						addCubeFace(vertexArray, cubeType, eCubeSide::Front, position);
					}
					else if (!forwardNeighbouringChunk)
					{
						regenChunk = true;
					}

					//Back Face
					glm::ivec3 backPosition(x, y, z - 1);
					if (chunk.isPositionInBounds(backPosition))
					{
						if (!isCubeAtPosition(backPosition, chunk))
						{
							addCubeFace(vertexArray, cubeType, eCubeSide::Back, position);
						}
					}
					else if (backNeighbouringChunk &&
						!isCubeAtPosition(backPosition, *backNeighbouringChunk))
					{
						addCubeFace(vertexArray, cubeType, eCubeSide::Back, position);
					}
					else if (!backNeighbouringChunk)
					{
						regenChunk = true;
					}

					//Top Face
					if (y == Utilities::CHUNK_HEIGHT - 1 || !isCubeAtPosition(glm::ivec3(x, y + 1, z), chunk))
					{
						if (!vertexArray.m_awaitingRegeneration)
						{
							addCubeFace(vertexArray, cubeType, eCubeSide::Top, position);
						}
					}

					//Bottom Face
					if (cubeType == eCubeType::Leaves)
					{
						addCubeFace(vertexArray, cubeType, eCubeSide::Bottom, position);
					}
				}
			}
		}
	}

	if (!vertexArray.m_awaitingRegeneration && regenChunk)
	{
		vertexArray.m_awaitingRegeneration = true;
	}
	else if (vertexArray.m_awaitingRegeneration && !regenChunk)
	{
		if (!vertexArray.m_vertexBuffer.indicies.empty())
		{
			vertexArray.m_attachOpaqueVBO = true;
		}

		if (!vertexArray.m_vertexBuffer.transparentIndicies.empty())
		{
			vertexArray.m_attachTransparentVBO = true;
		}

		vertexArray.m_awaitingRegeneration = false;
	}
	else if (!vertexArray.m_awaitingRegeneration && !regenChunk)
	{
		if (!vertexArray.m_vertexBuffer.indicies.empty())
		{
			vertexArray.m_attachOpaqueVBO = true;
		}

		if (!vertexArray.m_vertexBuffer.transparentIndicies.empty())
		{
			vertexArray.m_attachTransparentVBO = true;
		}
	}
}

void ChunkManager::deleteChunks(const glm::ivec3& playerPosition, std::mutex& renderingMutex)
{
	glm::ivec3 startingPosition(playerPosition);
	Utilities::getClosestMiddlePosition(startingPosition);
	Rectangle visibilityRect(glm::vec2(startingPosition.x, startingPosition.z), Utilities::VISIBILITY_DISTANCE);

	std::lock_guard<std::mutex> lock(renderingMutex);
	//Locate Chunks to delete
	for (auto chunk = m_chunks.begin(); chunk != m_chunks.end();)
	{
		const glm::ivec3& chunkStartingPosition = chunk->second.object->getStartingPosition();
		if (!visibilityRect.contains(chunk->second.object->getAABB()))
		{
			auto VAO = m_VAOs.find(chunkStartingPosition);
			if (VAO != m_VAOs.cend())
			{
				m_VAOs.erase(VAO);
			}
			else
			{
				auto regenerate = m_regenerate.find(chunkStartingPosition);
				assert(regenerate != m_regenerate.cend());
				if (regenerate != m_regenerate.cend())
				{
					m_regenerate.erase(regenerate);
				}
			}

			chunk = m_chunks.erase(chunk);
		}
		else
		{
			++chunk;
		}
	}
}

void ChunkManager::addChunks(const glm::vec3& playerPosition, std::mutex& renderingMutex)
{
	std::vector<const Chunk*> addedChunks;
	glm::ivec3 startPosition(playerPosition);
	Utilities::getClosestMiddlePosition(startPosition);
	for (int z = startPosition.z - Utilities::VISIBILITY_DISTANCE; z <= startPosition.z + Utilities::VISIBILITY_DISTANCE; z += Utilities::CHUNK_DEPTH)
	{
		for (int x = startPosition.x - Utilities::VISIBILITY_DISTANCE; x <= startPosition.x + Utilities::VISIBILITY_DISTANCE; x += Utilities::CHUNK_WIDTH)
		{
			if (x >= startPosition.x - (Utilities::VISIBILITY_DISTANCE - Utilities::CHUNK_WIDTH) && 
				x <= startPosition.x + (Utilities::VISIBILITY_DISTANCE - Utilities::CHUNK_WIDTH) &&
				z >= startPosition.z - (Utilities::VISIBILITY_DISTANCE - Utilities::CHUNK_DEPTH) && 
				z <= startPosition.z + (Utilities::VISIBILITY_DISTANCE - Utilities::CHUNK_DEPTH))
			{
				continue;
			}

			glm::ivec3 chunkStartingPosition(x, 0, z);
			Utilities::getClosestChunkStartingPosition(chunkStartingPosition);
			if (m_chunks.find(chunkStartingPosition) == m_chunks.cend())
			{
				const Chunk* chunk = m_chunks.emplace(std::piecewise_construct,
					std::forward_as_tuple(chunkStartingPosition),
					std::forward_as_tuple(m_chunkPool, chunkStartingPosition)).first->second.object;
				
				addedChunks.push_back(chunk);
			}
		}
	}

	for (const auto& addedChunk : addedChunks)
	{
		VertexArrayFromPool VAO(m_vertexArrayPool);
		generateChunkMesh(*VAO.object, *addedChunk);

		m_regenerate.emplace(std::piecewise_construct,
			std::forward_as_tuple(addedChunk->getStartingPosition()),
			std::forward_as_tuple(*addedChunk, std::move(VAO)));
	}
}

void ChunkManager::handleRegeneration(std::mutex& renderingMutex)
{
	for (auto regen = m_regenerate.begin(); regen != m_regenerate.end();)
	{
		if (regen->second.chunkToRegenerate.isInUse())
		{
			//If Chunk has no neighbours - then it can be regenerated
			const glm::ivec3& chunkStartingPosition = regen->second.chunkToRegenerate.getStartingPosition();
			if (m_chunks.find(Utilities::getNeighbouringChunkPosition(chunkStartingPosition, eDirection::Left)) != m_chunks.cend() &&
				m_chunks.find(Utilities::getNeighbouringChunkPosition(chunkStartingPosition, eDirection::Right)) != m_chunks.cend() &&
				m_chunks.find(Utilities::getNeighbouringChunkPosition(chunkStartingPosition, eDirection::Back)) != m_chunks.cend() &&
				m_chunks.find(Utilities::getNeighbouringChunkPosition(chunkStartingPosition, eDirection::Forward)) != m_chunks.cend())
			{
				generateChunkMesh(*regen->second.vertexArrayToRegenerate.object, regen->second.chunkToRegenerate);
				regen->second.regenerated = true;
			}
		
			++regen;
		}
		else
		{
			regen = m_regenerate.erase(regen);
		}
	}

	std::lock_guard<std::mutex> renderingLock(renderingMutex);
	for (auto iter = m_regenerate.begin(); iter != m_regenerate.end();)
	{
		if (iter->second.regenerated)
		{
			const glm::ivec3& chunkStartingPosition = iter->second.chunkToRegenerate.getStartingPosition();
			
			m_VAOs.emplace(std::piecewise_construct,
				std::forward_as_tuple(chunkStartingPosition), 
				std::forward_as_tuple(std::move(iter->second.vertexArrayToRegenerate)));

			iter = m_regenerate.erase(iter);
		}
		else
		{
			++iter;
		}
	}
}

const Chunk* ChunkManager::getNeighbouringChunkAtPosition(const glm::ivec3& chunkStartingPosition, eDirection direction) const
{
	const Chunk* neighbouringChunk = nullptr;

	switch (direction)
	{
	case eDirection::Left :
	{
		auto chunk = m_chunks.find(glm::ivec3(chunkStartingPosition.x - Utilities::CHUNK_WIDTH, chunkStartingPosition.y, chunkStartingPosition.z));
		if (chunk != m_chunks.cend())
		{
			neighbouringChunk = chunk->second.object;
		}
		break;
	}
	case eDirection::Right :
	{
		auto chunk = m_chunks.find(glm::ivec3(chunkStartingPosition.x + Utilities::CHUNK_WIDTH, chunkStartingPosition.y, chunkStartingPosition.z));
		if (chunk != m_chunks.cend())
		{
			neighbouringChunk = chunk->second.object;
		}
		break;
	}
	case eDirection::Forward :
	{
		auto chunk = m_chunks.find(glm::ivec3(chunkStartingPosition.x, chunkStartingPosition.y, chunkStartingPosition.z + Utilities::CHUNK_DEPTH));
		if (chunk != m_chunks.cend())
		{
			neighbouringChunk = chunk->second.object;
		}
		break;
	}
	case eDirection::Back :
	{
		auto chunk = m_chunks.find(glm::ivec3(chunkStartingPosition.x, chunkStartingPosition.y, chunkStartingPosition.z - Utilities::CHUNK_DEPTH));
		if (chunk != m_chunks.cend())
		{
			neighbouringChunk = chunk->second.object;
		}
		break;
	}
	default: 
		assert(false);
	}

	return neighbouringChunk;
}