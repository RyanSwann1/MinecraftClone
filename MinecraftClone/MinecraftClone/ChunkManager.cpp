#include "ChunkManager.h"
#include "Utilities.h"
#include "VertexBuffer.h"
#include "Texture.h"
#include "VertexArray.h"
#include "CubeID.h"
#include "Rectangle.h"
#include "Camera.h"
#include <iostream>

ChunkManager::ChunkManager(std::shared_ptr<Texture>& texture)
	: m_chunkPool(),
	m_texture(texture),
	m_VAOs(),
	m_chunks(),
	m_chunksToRegenerate(),
	m_mutex()
{}

std::unordered_map<glm::ivec3, VertexArray>& ChunkManager::getVAOs()
{
	return m_VAOs;
}

void ChunkManager::generateInitialChunks(const glm::vec3& playerPosition)
{
	for (int z = playerPosition.z - Utilities::VISIBILITY_DISTANCE; z < playerPosition.z + Utilities::VISIBILITY_DISTANCE; z += Utilities::CHUNK_DEPTH)
	{
		for (int x = playerPosition.x - Utilities::VISIBILITY_DISTANCE; x < playerPosition.x + Utilities::VISIBILITY_DISTANCE; x += Utilities::CHUNK_WIDTH)
		{
			glm::ivec3 chunkStartingPosition = Utilities::getClosestChunkStartingPosition(glm::ivec3(x, 0, z));
			if (m_chunks.find(chunkStartingPosition) == m_chunks.cend())
			{
				m_VAOs.emplace(std::piecewise_construct,
					std::forward_as_tuple(chunkStartingPosition),
					std::forward_as_tuple());

				m_chunks.emplace(std::piecewise_construct,
					std::forward_as_tuple(chunkStartingPosition),
					std::forward_as_tuple(m_chunkPool, chunkStartingPosition));

			}
		}
	}

	for (const auto& chunk : m_chunks)
	{
		auto VAO = m_VAOs.find(chunk.second.chunk.getStartingPosition());
		assert(VAO != m_VAOs.cend());
		if (VAO != m_VAOs.cend())
		{
			generateChunkMesh(VAO->second, chunk.second.chunk);
		}
	}
}

void ChunkManager::update(Rectangle& visibilityRect, const Camera& camera, const sf::Window& window)
{
	while (window.isOpen())
	{
		visibilityRect.update(glm::vec2(camera.m_position.x, camera.m_position.z), Utilities::VISIBILITY_DISTANCE);

		deleteChunks(visibilityRect);
		addChunks(visibilityRect, camera.m_position);
		regenChunks(visibilityRect, camera.m_position);
	}
}

void ChunkManager::addCubeFace(VertexArray& vertexArray, eCubeType cubeType, eCubeSide cubeSide, const glm::ivec3& cubePosition)
{
	if (cubeType == eCubeType::Water)
	{
		assert(cubeSide == eCubeSide::Top);
		if (cubeSide == eCubeSide::Top)
		{
			for (glm::vec3 i : Utilities::CUBE_FACE_TOP)
			{
				i += cubePosition;

				vertexArray.m_vertexBuffer.transparentPositions.push_back({ i.x, i.y, i.z });
			}

			m_texture->getTextCoords(eCubeFaceID::Water, vertexArray.m_vertexBuffer.transparentTextCoords);
			
			for (unsigned int i : Utilities::CUBE_FACE_INDICIES)
			{
				vertexArray.m_vertexBuffer.transparentIndicies.push_back(i + vertexArray.m_transparentElementBufferIndex);
			}
		}

		vertexArray.m_transparentElementBufferIndex += Utilities::CUBE_FACE_INDICIE_COUNT;
	}
	else
	{
		switch (cubeSide)
		{
		case eCubeSide::Front:
			for (glm::vec3 i : Utilities::CUBE_FACE_FRONT)
			{
				i += cubePosition;

				vertexArray.m_vertexBuffer.positions.push_back({ i.x, i.y, i.z });
			}

			switch (cubeType)
			{
			case eCubeType::Stone:
				m_texture->getTextCoords(eCubeFaceID::Stone, vertexArray.m_vertexBuffer.textCoords);
				break;
			case eCubeType::Sand:
				m_texture->getTextCoords(eCubeFaceID::Sand, vertexArray.m_vertexBuffer.textCoords);
				break;
			case eCubeType::Grass:
				m_texture->getTextCoords(eCubeFaceID::GrassSide, vertexArray.m_vertexBuffer.textCoords);
				break;
			case eCubeType::TreeStump:
				m_texture->getTextCoords(eCubeFaceID::TreeStump, vertexArray.m_vertexBuffer.textCoords);
				break;
			case eCubeType::Leaves:
				m_texture->getTextCoords(eCubeFaceID::Leaves, vertexArray.m_vertexBuffer.textCoords);
				break;
			}
			break;
		case eCubeSide::Back:
			for (glm::vec3 i : Utilities::CUBE_FACE_BACK)
			{
				i += cubePosition;
				vertexArray.m_vertexBuffer.positions.push_back({ i.x, i.y, i.z });
			}

			switch (cubeType)
			{
			case eCubeType::Stone:
				m_texture->getTextCoords(eCubeFaceID::Stone, vertexArray.m_vertexBuffer.textCoords);
				break;
			case eCubeType::Sand:
				m_texture->getTextCoords(eCubeFaceID::Sand, vertexArray.m_vertexBuffer.textCoords);
				break;
			case eCubeType::Grass:
				m_texture->getTextCoords(eCubeFaceID::GrassSide, vertexArray.m_vertexBuffer.textCoords);
				break;
			case eCubeType::TreeStump:
				m_texture->getTextCoords(eCubeFaceID::TreeStump, vertexArray.m_vertexBuffer.textCoords);
				break;
			case eCubeType::Leaves:
				m_texture->getTextCoords(eCubeFaceID::Leaves, vertexArray.m_vertexBuffer.textCoords);
				break;
			}
			break;
		case eCubeSide::Left:
			for (glm::vec3 i : Utilities::CUBE_FACE_LEFT)
			{
				i += cubePosition;
				vertexArray.m_vertexBuffer.positions.push_back({ i.x, i.y, i.z });
			}

			switch (cubeType)
			{
			case eCubeType::Stone:
				m_texture->getTextCoords(eCubeFaceID::Stone, vertexArray.m_vertexBuffer.textCoords);
				break;
			case eCubeType::Sand:
				m_texture->getTextCoords(eCubeFaceID::Sand, vertexArray.m_vertexBuffer.textCoords);
				break;
			case eCubeType::Grass:
				m_texture->getTextCoords(eCubeFaceID::GrassSide, vertexArray.m_vertexBuffer.textCoords);
				break;
			case eCubeType::TreeStump:
				m_texture->getTextCoords(eCubeFaceID::TreeStump, vertexArray.m_vertexBuffer.textCoords);
				break;
			case eCubeType::Leaves:
				m_texture->getTextCoords(eCubeFaceID::Leaves, vertexArray.m_vertexBuffer.textCoords);
				break;
			}
			break;
		case eCubeSide::Right:
			for (glm::vec3 i : Utilities::CUBE_FACE_RIGHT)
			{
				i += cubePosition;
				vertexArray.m_vertexBuffer.positions.push_back({ i.x, i.y, i.z });
			}

			switch (cubeType)
			{
			case eCubeType::Stone:
				m_texture->getTextCoords(eCubeFaceID::Stone, vertexArray.m_vertexBuffer.textCoords);
				break;
			case eCubeType::Sand:
				m_texture->getTextCoords(eCubeFaceID::Sand, vertexArray.m_vertexBuffer.textCoords);
				break;
			case eCubeType::Grass:
				m_texture->getTextCoords(eCubeFaceID::GrassSide, vertexArray.m_vertexBuffer.textCoords);
				break;
			case eCubeType::TreeStump:
				m_texture->getTextCoords(eCubeFaceID::TreeStump, vertexArray.m_vertexBuffer.textCoords);
				break;
			case eCubeType::Leaves:
				m_texture->getTextCoords(eCubeFaceID::Leaves, vertexArray.m_vertexBuffer.textCoords);
				break;
			}
			break;
		case eCubeSide::Top:
			for (glm::vec3 i : Utilities::CUBE_FACE_TOP)
			{
				i += cubePosition;
				vertexArray.m_vertexBuffer.positions.push_back({ i.x, i.y, i.z });
			}

			switch (cubeType)
			{
			case eCubeType::Stone:
				m_texture->getTextCoords(eCubeFaceID::Stone, vertexArray.m_vertexBuffer.textCoords);
				break;
			case eCubeType::Sand:
				m_texture->getTextCoords(eCubeFaceID::Sand, vertexArray.m_vertexBuffer.textCoords);
				break;
			case eCubeType::Grass:
				m_texture->getTextCoords(eCubeFaceID::Grass, vertexArray.m_vertexBuffer.textCoords);
				break;
			case eCubeType::TreeStump:
				m_texture->getTextCoords(eCubeFaceID::TreeStump, vertexArray.m_vertexBuffer.textCoords);
				break;
			case eCubeType::Leaves:
				m_texture->getTextCoords(eCubeFaceID::Leaves, vertexArray.m_vertexBuffer.textCoords);
				break;
			}
			break;
		case eCubeSide::Bottom:
			for (glm::vec3 i : Utilities::CUBE_FACE_BOTTOM)
			{
				i += cubePosition;
				vertexArray.m_vertexBuffer.positions.push_back({ i.x, i.y, i.z });
			}	

			switch (cubeType)
			{
			case eCubeType::Stone:
				m_texture->getTextCoords(eCubeFaceID::Stone, vertexArray.m_vertexBuffer.textCoords);
				break;
			case eCubeType::Sand:
				m_texture->getTextCoords(eCubeFaceID::Sand, vertexArray.m_vertexBuffer.textCoords);
				break;
			case eCubeType::Grass:
				m_texture->getTextCoords(eCubeFaceID::Sand, vertexArray.m_vertexBuffer.textCoords);
				break;
			case eCubeType::TreeStump:
				m_texture->getTextCoords(eCubeFaceID::TreeStump, vertexArray.m_vertexBuffer.textCoords);
				break;
			case eCubeType::Leaves:
				m_texture->getTextCoords(eCubeFaceID::Leaves, vertexArray.m_vertexBuffer.textCoords);
				break;
			}
			break;
		}

		for (unsigned int i : Utilities::CUBE_FACE_INDICIES)
		{
			vertexArray.m_vertexBuffer.indicies.push_back(i + vertexArray.m_opaqueElementBufferIndex);
		}

		vertexArray.m_opaqueElementBufferIndex += Utilities::CUBE_FACE_INDICIE_COUNT;
	}
}

bool ChunkManager::isCubeAtPosition(const glm::ivec3& position) const
{
	auto cIter = m_chunks.find(Utilities::getClosestChunkStartingPosition(position));
	if (cIter != m_chunks.cend() && cIter->second.chunk.isPositionInBounds(position) && 
		static_cast<eCubeType>(cIter->second.chunk.getCubeDetailsWithoutBoundsCheck(position).type) != eCubeType::Invalid && 
		static_cast<eCubeType>(cIter->second.chunk.getCubeDetailsWithoutBoundsCheck(position).type) != eCubeType::Water)
	{
		return true;
	}

	return false;
}

bool ChunkManager::isCubeAtPosition(const glm::ivec3& position, const Chunk& chunk) const
{
	const CubeDetails& cubeDetails = chunk.getCubeDetailsWithoutBoundsCheck(position);
	return (cubeDetails.type != static_cast<char>(eCubeType::Invalid) && cubeDetails.type != static_cast<char>(eCubeType::Water) ? true : false);
}

bool ChunkManager::isChunkAtPosition(const glm::ivec3& position) const
{
	auto cIter = m_chunks.find(Utilities::getClosestChunkStartingPosition(position));
	return cIter != m_chunks.cend();
}

void ChunkManager::generateChunkMesh(VertexArray& vertexArray, const Chunk& chunk)
{
	const glm::ivec3& chunkStartingPosition = chunk.getStartingPosition();
	bool regenChunk = false;

	const Chunk* leftNeighbouringChunk =
		getNeighbouringChunkAtPosition(glm::ivec3(chunkStartingPosition.x - Utilities::CHUNK_WIDTH, chunkStartingPosition.y, chunkStartingPosition.z));

	const Chunk* rightNeighbouringChunk =
		getNeighbouringChunkAtPosition(glm::ivec3(chunkStartingPosition.x + Utilities::CHUNK_WIDTH, chunkStartingPosition.y, chunkStartingPosition.z));

	const Chunk* forwardNeighbouringChunk = 
		getNeighbouringChunkAtPosition(glm::ivec3(chunkStartingPosition.x, chunkStartingPosition.y, chunkStartingPosition.z + Utilities::CHUNK_DEPTH));

	const Chunk* backNeighbouringChunk =
		getNeighbouringChunkAtPosition(glm::ivec3(chunkStartingPosition.x, chunkStartingPosition.y, chunkStartingPosition.z - Utilities::CHUNK_DEPTH));

	for (int z = chunkStartingPosition.z; z < chunkStartingPosition.z + Utilities::CHUNK_DEPTH; ++z)
	{
		for (int y = chunkStartingPosition.y; y < chunkStartingPosition.y + Utilities::CHUNK_HEIGHT; ++y)
		{
			for (int x = chunkStartingPosition.x; x < chunkStartingPosition.x + Utilities::CHUNK_WIDTH; ++x)
			{
				glm::ivec3 position(x, y, z);
				eCubeType cubeType = static_cast<eCubeType>(chunk.getCubeDetailsWithoutBoundsCheck(position).type);
				if (cubeType == eCubeType::Invalid)
				{
					continue;
				}
				else if (cubeType == eCubeType::Water)
				{
					addCubeFace(vertexArray, cubeType, eCubeSide::Top, position);
				}
				else
				{
					//Left
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
					
					//Right
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

					//Forward
					glm::ivec3 forwardPosition(x, y, z + 1);
					if (chunk.isPositionInBounds(forwardPosition))
					{
						if (!isCubeAtPosition(forwardPosition, chunk))
						{
							addCubeFace(vertexArray, cubeType, eCubeSide::Front, position);
						}
					}
					else if ( forwardNeighbouringChunk &&
						!isCubeAtPosition(forwardPosition, *forwardNeighbouringChunk))
					{
						addCubeFace(vertexArray, cubeType, eCubeSide::Front, position);
					}
					else if (!forwardNeighbouringChunk)
					{
						regenChunk = true;
					}

					//Back
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

					//Top
					if (cubeType != eCubeType::TreeStump)
					{
						if (y < Utilities::CHUNK_HEIGHT - 1 && !isCubeAtPosition(glm::ivec3(x, y + 1, z), chunk))
						{
							addCubeFace(vertexArray, cubeType, eCubeSide::Top, position);
						}
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
		vertexArray.m_awaitingRegeneration = false;
	}

	vertexArray.m_attachOpaqueVBO = true;
	vertexArray.m_attachTransparentVBO = true;
}

void ChunkManager::deleteChunks(const Rectangle& visibilityRect)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	for (auto chunk = m_chunks.begin(); chunk != m_chunks.end();)
	{
		Rectangle chunkAABB(glm::ivec2(chunk->second.chunk.getStartingPosition().x, chunk->second.chunk.getStartingPosition().z) +
			glm::ivec2(Utilities::CHUNK_WIDTH / 2.0f, Utilities::CHUNK_DEPTH / 2.0f), 16);
		if (!visibilityRect.contains(chunkAABB))
		{
			const glm::ivec3& chunkStartingPosition = chunk->second.chunk.getStartingPosition();
			auto VAO = m_VAOs.find(chunkStartingPosition);
			assert(VAO != m_VAOs.end());
			if (VAO != m_VAOs.end())
			{
				VAO->second.m_destroy = true;
			}

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

void ChunkManager::addChunks(const Rectangle& visibilityRect, const glm::vec3& playerPosition)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	std::queue<const Chunk*> newlyAddedChunks;
	glm::ivec3 startPosition = Utilities::getClosestChunkStartingPosition(playerPosition);	
	for (int z = startPosition.z - Utilities::VISIBILITY_DISTANCE; z < startPosition.z + Utilities::VISIBILITY_DISTANCE; z += Utilities::CHUNK_DEPTH)
	{
		for (int x = startPosition.x - Utilities::VISIBILITY_DISTANCE; x < startPosition.x + Utilities::VISIBILITY_DISTANCE; x += Utilities::CHUNK_WIDTH)
		{
			glm::ivec3 position(x, 0, z);
			if (m_chunks.find(position) == m_chunks.cend())
			{
				assert(m_VAOs.find(position) == m_VAOs.cend());
				auto newVAO = m_VAOs.emplace(std::piecewise_construct,
					std::forward_as_tuple(position),
					std::forward_as_tuple()).first;

				auto newChunk = m_chunks.emplace(std::piecewise_construct,
					std::forward_as_tuple(position),
					std::forward_as_tuple(m_chunkPool, position)).first;

				newlyAddedChunks.push(&newChunk->second.chunk);
			}
		}
	}

	while (!newlyAddedChunks.empty())
	{
		const Chunk* newChunk = newlyAddedChunks.front();
		newlyAddedChunks.pop();

		auto VAO = m_VAOs.find(newChunk->getStartingPosition());
		assert(VAO != m_VAOs.end());
		if (VAO != m_VAOs.end())
		{
			generateChunkMesh(VAO->second, *newChunk);
		}
	}
}

void ChunkManager::regenChunks(const Rectangle& visibilityRect, const glm::vec3& playerPosition)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	for (auto chunkStartingPosition = m_chunksToRegenerate.begin(); chunkStartingPosition != m_chunksToRegenerate.end();)
	{
		auto chunk = m_chunks.find(*chunkStartingPosition);
		if (chunk != m_chunks.cend())
		{
			if (m_chunks.find(glm::ivec3(chunkStartingPosition->x - Utilities::CHUNK_WIDTH, 0, chunkStartingPosition->z)) != m_chunks.cend() &&
				m_chunks.find(glm::ivec3(chunkStartingPosition->x + Utilities::CHUNK_WIDTH, 0, chunkStartingPosition->z)) != m_chunks.cend() &&
				m_chunks.find(glm::ivec3(chunkStartingPosition->x, 0, chunkStartingPosition->z - Utilities::CHUNK_DEPTH)) != m_chunks.cend() &&
				m_chunks.find(glm::ivec3(chunkStartingPosition->x, 0, chunkStartingPosition->z + Utilities::CHUNK_DEPTH)) != m_chunks.cend())
			{
				auto VAO = m_VAOs.find(*chunkStartingPosition);
				assert(VAO != m_VAOs.end());
				if (VAO != m_VAOs.end())
				{
					generateChunkMesh(VAO->second, chunk->second.chunk);
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

const Chunk* ChunkManager::getNeighbouringChunkAtPosition(const glm::ivec3& chunkStartingPosition) const
{
	auto chunk = m_chunks.find(chunkStartingPosition);
	return (chunk != m_chunks.cend() ? &chunk->second.chunk : nullptr);
}
