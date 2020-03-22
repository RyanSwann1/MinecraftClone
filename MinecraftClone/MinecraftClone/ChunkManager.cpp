#include "ChunkManager.h"
#include "Utilities.h"
#include "VertexBuffer.h"
#include "VertexArray.h"
#include "CubeID.h"
#include "Rectangle.h"
#include "Camera.h"
#include <iostream>

ChunkManager::ChunkManager()
	: m_resetting(false),
	m_chunkPool(),
	m_vertexArrayPool(),
	m_VAOs(),
	m_chunks(),
	m_chunksToRegenerate(),
	m_mutex()
{}

void ChunkManager::reset()
{
	assert(!m_resetting);
	std::lock_guard<std::mutex> lock(m_mutex);
	m_resetting = true;
}

void ChunkManager::reset(const glm::vec3& playerPosition)
{
	assert(m_resetting);
	std::lock_guard<std::mutex> lock(m_mutex);
	m_chunks.clear();
	m_VAOs.clear();
	m_chunksToRegenerate.clear();

	generateInitialChunks(playerPosition);
	m_resetting = false;
}

bool ChunkManager::isResetting() const
{
	return m_resetting;
}

std::unordered_map<glm::ivec3, VertexArrayFromPool>& ChunkManager::getVAOs()
{
	return m_VAOs;
}

void ChunkManager::generateInitialChunks(const glm::vec3& playerPosition)
{
	std::queue<std::pair<const Chunk*, VertexArray*>> recentlyAddedQueue;
	for (int z = playerPosition.z - Utilities::VISIBILITY_DISTANCE; z <= playerPosition.z + Utilities::VISIBILITY_DISTANCE; z += Utilities::CHUNK_DEPTH)
	{
		for (int x = playerPosition.x - Utilities::VISIBILITY_DISTANCE; x <= playerPosition.x + Utilities::VISIBILITY_DISTANCE; x += Utilities::CHUNK_WIDTH)
		{
			glm::ivec3 chunkStartingPosition(x, 0, z);
			Utilities::getClosestChunkStartingPosition(chunkStartingPosition);
			assert(m_chunks.find(chunkStartingPosition) == m_chunks.cend() && m_VAOs.find(chunkStartingPosition) == m_VAOs.cend());
			
			VertexArray& VAO = m_VAOs.emplace(std::piecewise_construct,
				std::forward_as_tuple(chunkStartingPosition),
				std::forward_as_tuple(m_vertexArrayPool)).first->second.object;

			const Chunk& chunk = m_chunks.emplace(std::piecewise_construct,
				std::forward_as_tuple(chunkStartingPosition),
				std::forward_as_tuple(m_chunkPool, chunkStartingPosition)).first->second.object;
		
			recentlyAddedQueue.push(std::make_pair<const Chunk*, VertexArray*>(&chunk, &VAO));
		}
	}

	while (!recentlyAddedQueue.empty())
	{
		std::pair<const Chunk*, VertexArray*> recentlyAdded = recentlyAddedQueue.front();
		recentlyAddedQueue.pop();
		generateChunkMesh(*recentlyAdded.second, *recentlyAdded.first);
	}
}

void ChunkManager::update(const Camera& camera, const sf::Window& window)
{
	while (window.isOpen())
	{
		if (m_resetting)
		{
			reset(camera.m_position);
		}

		addChunks(camera.m_position);
		deleteChunks(camera.m_position);
		regenChunks();

		std::this_thread::sleep_for(std::chrono::milliseconds(50));
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
		m_chunksToRegenerate.insert(chunkStartingPosition);
		vertexArray.m_awaitingRegeneration = true;
	}
	else if (vertexArray.m_awaitingRegeneration && !regenChunk)
	{
		vertexArray.m_attachOpaqueVBO = true;
		vertexArray.m_attachTransparentVBO = true;
		vertexArray.m_awaitingRegeneration = false;
	}
	else if (!vertexArray.m_awaitingRegeneration && !regenChunk)
	{
		vertexArray.m_attachOpaqueVBO = true;
		vertexArray.m_attachTransparentVBO = true;
	}
}

void ChunkManager::deleteChunks(const glm::ivec3& playerPosition)
{
	glm::ivec3 startingPosition(playerPosition);
	Utilities::getClosestMiddlePosition(startingPosition);
	Rectangle visibilityRect(glm::vec2(startingPosition.x, startingPosition.z), Utilities::VISIBILITY_DISTANCE);
	for (auto chunk = m_chunks.begin(); chunk != m_chunks.end();)
	{
		if (!visibilityRect.contains(chunk->second.object.getAABB()))
		{
			const glm::ivec3& chunkStartingPosition = chunk->second.object.getStartingPosition();
			
			std::unique_lock<std::mutex> lock(m_mutex);
			auto VAO = m_VAOs.find(chunkStartingPosition);
			assert(VAO != m_VAOs.end());
			if (VAO != m_VAOs.end())
			{
				assert(!VAO->second.object.m_reset);
				VAO->second.object.m_reset = true;
			}
			lock.unlock();

			auto chunkToRegen = std::find_if(m_chunksToRegenerate.begin(), m_chunksToRegenerate.end(), [chunkStartingPosition](const auto& position)
			{
				return position == chunkStartingPosition;
			});
			if (chunkToRegen != m_chunksToRegenerate.end())
			{
				m_chunksToRegenerate.erase(chunkToRegen);
			}

			chunk = m_chunks.erase(chunk);
		}
		else
		{
			++chunk;
		}
	}
}

void ChunkManager::addChunks(const glm::vec3& playerPosition)
{
	std::queue<std::pair<const Chunk*, VertexArray*>> recentlyAddedQueue;
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
			
			glm::ivec3 position(x, 0, z);
			Utilities::getClosestChunkStartingPosition(position);
			if (m_chunks.find(position) == m_chunks.cend() && m_VAOs.find(position) == m_VAOs.cend())
			{
				std::unique_lock<std::mutex> lock(m_mutex);
				VertexArray& VAO = m_VAOs.emplace(std::piecewise_construct,
					std::forward_as_tuple(position),
					std::forward_as_tuple(m_vertexArrayPool)).first->second.object;
				lock.unlock();

				const Chunk& chunk = m_chunks.emplace(std::piecewise_construct,
					std::forward_as_tuple(position),
					std::forward_as_tuple(m_chunkPool, position)).first->second.object;

				recentlyAddedQueue.push(std::make_pair<const Chunk*, VertexArray*>(&chunk, &VAO));
			}
		}
	}

	std::lock_guard<std::mutex> lock(m_mutex);
	while (!recentlyAddedQueue.empty())
	{
		std::pair<const Chunk*, VertexArray*> recentlyAdded = recentlyAddedQueue.front();
		recentlyAddedQueue.pop();
		generateChunkMesh(*recentlyAdded.second, *recentlyAdded.first);
	}
}

void ChunkManager::regenChunks()
{
	for (auto chunkStartingPosition = m_chunksToRegenerate.cbegin(); chunkStartingPosition != m_chunksToRegenerate.cend();)
	{
		auto chunk = m_chunks.find(*chunkStartingPosition);
		if (chunk != m_chunks.cend())
		{
			if (m_chunks.find(Utilities::getNeighbouringChunkPosition(*chunkStartingPosition, eDirection::Left)) != m_chunks.cend() &&
				m_chunks.find(Utilities::getNeighbouringChunkPosition(*chunkStartingPosition, eDirection::Right)) != m_chunks.cend() &&
				m_chunks.find(Utilities::getNeighbouringChunkPosition(*chunkStartingPosition, eDirection::Back)) != m_chunks.cend() &&
				m_chunks.find(Utilities::getNeighbouringChunkPosition(*chunkStartingPosition, eDirection::Forward)) != m_chunks.cend())
			{
				std::lock_guard<std::mutex> lock(m_mutex);

				auto VAO = m_VAOs.find(*chunkStartingPosition);
				assert(VAO != m_VAOs.end());
				if (VAO != m_VAOs.end())
				{
					generateChunkMesh(VAO->second.object, chunk->second.object);
				}

				chunkStartingPosition = m_chunksToRegenerate.erase(chunkStartingPosition);
			}
			else
			{
				++chunkStartingPosition;
			}
		}
		else
		{
			chunkStartingPosition = m_chunksToRegenerate.erase(chunkStartingPosition);
		}
	}
}

const Chunk* ChunkManager::getNeighbouringChunkAtPosition(const glm::ivec3& chunkStartingPosition, eDirection direction) const
{
	const Chunk* chunk = nullptr;

	switch (direction)
	{
	case eDirection::Left :
	{
		auto cIter = m_chunks.find(glm::ivec3(chunkStartingPosition.x - Utilities::CHUNK_WIDTH, chunkStartingPosition.y, chunkStartingPosition.z));
		if (cIter != m_chunks.cend())
		{
			chunk = &cIter->second.object;
		}
		break;
	}
	case eDirection::Right :
	{
		auto cIter = m_chunks.find(glm::ivec3(chunkStartingPosition.x + Utilities::CHUNK_WIDTH, chunkStartingPosition.y, chunkStartingPosition.z));
		if (cIter != m_chunks.cend())
		{
			chunk = &cIter->second.object;
		}
		break;
	}
	case eDirection::Forward :
	{
		auto cIter = m_chunks.find(glm::ivec3(chunkStartingPosition.x, chunkStartingPosition.y, chunkStartingPosition.z + Utilities::CHUNK_DEPTH));
		if (cIter != m_chunks.cend())
		{
			chunk = &cIter->second.object;
		}
		break;
	}
	case eDirection::Back :
	{
		auto cIter = m_chunks.find(glm::ivec3(chunkStartingPosition.x, chunkStartingPosition.y, chunkStartingPosition.z - Utilities::CHUNK_DEPTH));
		if (cIter != m_chunks.cend())
		{
			chunk = &cIter->second.object;
		}
		break;
	}
	default: 
		assert(false);
	}

	return chunk;
}