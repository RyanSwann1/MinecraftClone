#include "ChunkManager.h"
#include "Utilities.h"
#include "VertexBuffer.h"
#include "Texture.h"
#include "VertexArray.h"
#include "CubeID.h"
#include "Rectangle.h"
#include "Camera.h"
#include <iostream>

ChunkManager::ChunkManager()
	: m_chunks(),
	m_chunkMeshRegenerateQueue()
{}

void ChunkManager::generateInitialChunks(glm::vec3 playerPosition, std::unordered_map<glm::ivec3, VertexArray>& VAOs, const Texture& texture)
{
	for (int z = playerPosition.z - Utilities::VISIBILITY_DISTANCE; z < playerPosition.z + Utilities::VISIBILITY_DISTANCE; z += Utilities::CHUNK_DEPTH)
	{
		for (int x = playerPosition.x - Utilities::VISIBILITY_DISTANCE; x < playerPosition.x + Utilities::VISIBILITY_DISTANCE; x += Utilities::CHUNK_WIDTH)
		{
			glm::ivec3 chunkStartingPosition = Utilities::getClosestChunkStartingPosition(glm::ivec3(x, 0, z));
			if (m_chunks.find(chunkStartingPosition) == m_chunks.cend())
			{
				m_chunks.emplace(std::piecewise_construct,
					std::forward_as_tuple(chunkStartingPosition),
					std::forward_as_tuple(chunkStartingPosition));

				VAOs.emplace(std::piecewise_construct,
					std::forward_as_tuple(chunkStartingPosition),
					std::forward_as_tuple());
			}
		}
	}

	for (const auto& chunk : m_chunks)
	{
		auto VAO = VAOs.find(chunk.second.getStartingPosition());
		assert(VAO != VAOs.cend());
		if (VAO != VAOs.cend())
		{
			generateChunkMesh(VAO->second, texture, chunk.second);
		}
	}
}

void ChunkManager::update(Rectangle& visibilityRect, std::unordered_map<glm::ivec3, VertexArray>& VAOs, Camera& camera,
	const Texture& texture, const sf::Window& window)
{
	while (window.isOpen())
	{
		visibilityRect.update(glm::vec2(camera.m_position.x, camera.m_position.z), Utilities::VISIBILITY_DISTANCE);

		deleteChunks(visibilityRect, VAOs);
		addChunks(visibilityRect, VAOs, camera.m_position, texture);
		regenChunks(visibilityRect, VAOs, camera.m_position, texture);
	}
}

void ChunkManager::addCubeFace(VertexBuffer& vertexBuffer, const Texture& texture, CubeDetails cubeDetails, eCubeSide cubeSide,
	int& opaqueElementBufferIndex, int& transparentElementBufferIndex, glm::ivec3 cubePosition)
{
	if (static_cast<eCubeType>(cubeDetails.type) == eCubeType::Water)
	{
		assert(cubeSide == eCubeSide::Top);
		if (cubeSide == eCubeSide::Top)
		{
			for (glm::vec3 i : Utilities::CUBE_FACE_TOP)
			{
				i += cubePosition;

				vertexBuffer.transparentPositions.push_back({ i.x, i.y, i.z });
			}

			texture.getTextCoords(eCubeFaceID::Water, vertexBuffer.transparentTextCoords);
			
			for (unsigned int i : Utilities::CUBE_FACE_INDICIES)
			{
				vertexBuffer.transparentIndicies.push_back(i + transparentElementBufferIndex);
			}
		}

		transparentElementBufferIndex += Utilities::CUBE_FACE_INDICIE_COUNT;
	}
	else
	{
		switch (cubeSide)
		{
		case eCubeSide::Front:
			for (glm::vec3 i : Utilities::CUBE_FACE_FRONT)
			{
				i += cubePosition;

				vertexBuffer.positions.push_back({ i.x, i.y, i.z });
			}

			switch (static_cast<eCubeType>(cubeDetails.type))
			{
			case eCubeType::Stone:
				texture.getTextCoords(eCubeFaceID::Stone, vertexBuffer.textCoords);
				break;
			case eCubeType::Sand:
				texture.getTextCoords(eCubeFaceID::Sand, vertexBuffer.textCoords);
				break;
			case eCubeType::Grass:
				texture.getTextCoords(eCubeFaceID::GrassSide, vertexBuffer.textCoords);
				break;
			}
			break;
		case eCubeSide::Back:
			for (glm::vec3 i : Utilities::CUBE_FACE_BACK)
			{
				i += cubePosition;
				vertexBuffer.positions.push_back({ i.x, i.y, i.z });
			}

			switch (static_cast<eCubeType>(cubeDetails.type))
			{
			case eCubeType::Stone:
				texture.getTextCoords(eCubeFaceID::Stone, vertexBuffer.textCoords);
				break;
			case eCubeType::Sand:
				texture.getTextCoords(eCubeFaceID::Sand, vertexBuffer.textCoords);
				break;
			case eCubeType::Grass:
				texture.getTextCoords(eCubeFaceID::GrassSide, vertexBuffer.textCoords);
				break;
			}
			break;
		case eCubeSide::Left:
			for (glm::vec3 i : Utilities::CUBE_FACE_LEFT)
			{
				i += cubePosition;
				vertexBuffer.positions.push_back({ i.x, i.y, i.z });
			}

			switch (static_cast<eCubeType>(cubeDetails.type))
			{
			case eCubeType::Stone:
				texture.getTextCoords(eCubeFaceID::Stone, vertexBuffer.textCoords);
				break;
			case eCubeType::Sand:
				texture.getTextCoords(eCubeFaceID::Sand, vertexBuffer.textCoords);
				break;
			case eCubeType::Grass:
				texture.getTextCoords(eCubeFaceID::GrassSide, vertexBuffer.textCoords);
				break;
			}
			break;
		case eCubeSide::Right:
			for (glm::vec3 i : Utilities::CUBE_FACE_RIGHT)
			{
				i += cubePosition;
				vertexBuffer.positions.push_back({ i.x, i.y, i.z });
			}

			switch (static_cast<eCubeType>(cubeDetails.type))
			{
			case eCubeType::Stone:
				texture.getTextCoords(eCubeFaceID::Stone, vertexBuffer.textCoords);
				break;
			case eCubeType::Sand:
				texture.getTextCoords(eCubeFaceID::Sand, vertexBuffer.textCoords);
				break;
			case eCubeType::Grass:
				texture.getTextCoords(eCubeFaceID::GrassSide, vertexBuffer.textCoords);
				break;
			}
			break;
		case eCubeSide::Top:
			for (glm::vec3 i : Utilities::CUBE_FACE_TOP)
			{
				i += cubePosition;
				vertexBuffer.positions.push_back({ i.x, i.y, i.z });
			}

			switch (static_cast<eCubeType>(cubeDetails.type))
			{
			case eCubeType::Stone:
				texture.getTextCoords(eCubeFaceID::Stone, vertexBuffer.textCoords);
				break;
			case eCubeType::Sand:
				texture.getTextCoords(eCubeFaceID::Sand, vertexBuffer.textCoords);
				break;
			case eCubeType::Grass:
				texture.getTextCoords(eCubeFaceID::Grass, vertexBuffer.textCoords);
				break;
			}
			break;
		case eCubeSide::Bottom:
			for (glm::vec3 i : Utilities::CUBE_FACE_BOTTOM)
			{
				i += cubePosition;
				vertexBuffer.positions.push_back({ i.x, i.y, i.z });
			}

			switch (static_cast<eCubeType>(cubeDetails.type))
			{
			case eCubeType::Stone:
				texture.getTextCoords(eCubeFaceID::Stone, vertexBuffer.textCoords);
				break;
			case eCubeType::Sand:
				texture.getTextCoords(eCubeFaceID::Sand, vertexBuffer.textCoords);
				break;
			case eCubeType::Grass:
				texture.getTextCoords(eCubeFaceID::Sand, vertexBuffer.textCoords);
				break;
			}
			break;
		}

		for (unsigned int i : Utilities::CUBE_FACE_INDICIES)
		{
			vertexBuffer.indicies.push_back(i + opaqueElementBufferIndex);
		}

		opaqueElementBufferIndex += Utilities::CUBE_FACE_INDICIE_COUNT;
	}
}

bool ChunkManager::isCubeAtPosition(glm::ivec3 position) const
{
	glm::ivec3 closestChunkStartingPosition = Utilities::getClosestChunkStartingPosition(position);
	auto cIter = m_chunks.find(closestChunkStartingPosition);
	if (cIter != m_chunks.cend() && cIter->second.isPositionInBounds(position) && 
		static_cast<eCubeType>(cIter->second.getCubeDetails(position).type) != eCubeType::Invalid && 
		static_cast<eCubeType>(cIter->second.getCubeDetails(position).type) != eCubeType::Water)
	{
		return true;
	}

	return false;
}

bool ChunkManager::isCubeAtPosition(glm::ivec3 position, const Chunk& chunk) const
{
	if (chunk.getCubeDetailsAtPosition(position).type != static_cast<char>(eCubeType::Invalid) &&
		chunk.getCubeDetailsAtPosition(position).type != static_cast<char>(eCubeType::Water))
	{
		return true;
	}

	return false;
	//return chunk.getCubeDetailsAtPosition(position).type != (static_cast<char>(eCubeType::Invalid));
}

bool ChunkManager::isChunkAtPosition(glm::ivec3 position) const
{
	glm::ivec3 closestChunkStartingPosition = Utilities::getClosestChunkStartingPosition(position);
	auto cIter = m_chunks.find(closestChunkStartingPosition);
	return cIter != m_chunks.cend();
}

void ChunkManager::generateChunkMesh(VertexArray& vertexArray, const Texture& texture, const Chunk& chunk)
{
	int opaqueElementBufferIndex = 0;
	int transparentElementBufferIndex = 0;
	glm::ivec3 chunkStartingPosition = chunk.getStartingPosition();
	bool regenChunk = false;
	std::array<const Chunk*, static_cast<size_t>(eDirection::Total)> neighbouringChunks =
	{
		getNeighbouringChunkAtPosition(glm::ivec3(chunkStartingPosition.x - Utilities::CHUNK_WIDTH, chunkStartingPosition.y, chunkStartingPosition.z)),
		getNeighbouringChunkAtPosition(glm::ivec3(chunkStartingPosition.x + Utilities::CHUNK_WIDTH, chunkStartingPosition.y, chunkStartingPosition.z)),
		getNeighbouringChunkAtPosition(glm::ivec3(chunkStartingPosition.x, chunkStartingPosition.y, chunkStartingPosition.z + Utilities::CHUNK_DEPTH)),
		getNeighbouringChunkAtPosition(glm::ivec3(chunkStartingPosition.x, chunkStartingPosition.y, chunkStartingPosition.z - Utilities::CHUNK_DEPTH))
	};

	for (int z = chunkStartingPosition.z; z < chunkStartingPosition.z + Utilities::CHUNK_DEPTH; ++z)
	{
		for (int y = chunkStartingPosition.y; y < chunkStartingPosition.y + Utilities::CHUNK_HEIGHT; ++y)
		{
			for (int x = chunkStartingPosition.x; x < chunkStartingPosition.x + Utilities::CHUNK_WIDTH; ++x)
			{
				glm::ivec3 position(x, y, z);
				if (static_cast<eCubeType>(chunk.getCubeDetails(position).type) == eCubeType::Invalid)
				{
					continue;
				}
				else if (static_cast<eCubeType>(chunk.getCubeDetails(position).type) == eCubeType::Water)
				{
					if (!regenChunk && !isChunkAtPosition(glm::ivec3(x - 1, y, z)) || !isChunkAtPosition(glm::ivec3(x + 1, y, z)) ||
						!isChunkAtPosition(glm::ivec3(x, y, z - 1)) || !isChunkAtPosition(glm::ivec3(x, y, z + 1)))
					{
						regenChunk = true;
					}

					addCubeFace(vertexArray.m_vertexBuffer, texture, chunk.getCubeDetails(position), eCubeSide::Top, 
						opaqueElementBufferIndex, transparentElementBufferIndex, position);
				}
				else
				{
					//Left
					glm::ivec3 leftPosition(x - 1, y, z);
					if (chunk.isPositionInBounds(leftPosition))
					{
						if (!isCubeAtPosition(leftPosition, chunk))
						{
							addCubeFace(vertexArray.m_vertexBuffer, texture, chunk.getCubeDetails(position), eCubeSide::Left,
								opaqueElementBufferIndex, transparentElementBufferIndex, position);
						}
					}
					else if (neighbouringChunks[static_cast<int>(eDirection::Left)] &&
						!isCubeAtPosition(leftPosition, *neighbouringChunks[static_cast<int>(eDirection::Left)]))
					{
						addCubeFace(vertexArray.m_vertexBuffer, texture, chunk.getCubeDetails(position), eCubeSide::Left,
							opaqueElementBufferIndex, transparentElementBufferIndex, position);
					}
					else if (!neighbouringChunks[static_cast<int>(eDirection::Left)])
					{

					}
					
					//Right
					glm::ivec3 rightPosition(x + 1, y, z);
					if (chunk.isPositionInBounds(rightPosition))
					{
						if (!isCubeAtPosition(rightPosition, chunk))
						{
							addCubeFace(vertexArray.m_vertexBuffer, texture, chunk.getCubeDetails(position), eCubeSide::Right,
								opaqueElementBufferIndex, transparentElementBufferIndex, position);
						}
					}
					else if (neighbouringChunks[static_cast<int>(eDirection::Right)] &&
						!isCubeAtPosition(rightPosition, *neighbouringChunks[static_cast<int>(eDirection::Right)]))
					{
						addCubeFace(vertexArray.m_vertexBuffer, texture, chunk.getCubeDetails(position), eCubeSide::Right,
							opaqueElementBufferIndex, transparentElementBufferIndex, position);
					}
					else if (!neighbouringChunks[static_cast<int>(eDirection::Right)])
					{

					}

					//Forward
					glm::ivec3 forwardPosition(x, y, z + 1);
					if (chunk.isPositionInBounds(forwardPosition))
					{
						if (!isCubeAtPosition(forwardPosition, chunk))
						{
							addCubeFace(vertexArray.m_vertexBuffer, texture, chunk.getCubeDetails(position), eCubeSide::Front,
								opaqueElementBufferIndex, transparentElementBufferIndex, position);
						}
					}
					else if (neighbouringChunks[static_cast<int>(eDirection::Forward)] &&
						!isCubeAtPosition(forwardPosition, *neighbouringChunks[static_cast<int>(eDirection::Forward)]))
					{
						addCubeFace(vertexArray.m_vertexBuffer, texture, chunk.getCubeDetails(position), eCubeSide::Front,
							opaqueElementBufferIndex, transparentElementBufferIndex, position);
					}
					else if (!neighbouringChunks[static_cast<int>(eDirection::Forward)])
					{

					}

					//Back
					glm::ivec3 backPosition(x, y, z - 1);
					if (chunk.isPositionInBounds(backPosition))
					{
						if (!isCubeAtPosition(backPosition, chunk))
						{
							addCubeFace(vertexArray.m_vertexBuffer, texture, chunk.getCubeDetails(position), eCubeSide::Back,
								opaqueElementBufferIndex, transparentElementBufferIndex, position);
						}
					}
					else if (neighbouringChunks[static_cast<int>(eDirection::Back)] &&
						!isCubeAtPosition(backPosition, *neighbouringChunks[static_cast<int>(eDirection::Back)]))
					{
						addCubeFace(vertexArray.m_vertexBuffer, texture, chunk.getCubeDetails(position), eCubeSide::Back,
							opaqueElementBufferIndex, transparentElementBufferIndex, position);
					}
					else if (neighbouringChunks[static_cast<int>(eDirection::Back)])
					{

					}

					//Top
					if (y < Utilities::CHUNK_HEIGHT && !isCubeAtPosition(glm::ivec3(x, y + 1, z), chunk))
					{
						addCubeFace(vertexArray.m_vertexBuffer, texture, chunk.getCubeDetails(position), eCubeSide::Top,
							opaqueElementBufferIndex, transparentElementBufferIndex, position);
					}

				/*	if (isChunkAtPosition(glm::ivec3(x - 1, y, z)) && !isCubeAtPosition(glm::ivec3(x - 1, y, z)))
					{
						addCubeFace(vertexArray.m_vertexBuffer, texture, chunk.getCubeDetails(position), eCubeSide::Left, 
							opaqueElementBufferIndex, transparentElementBufferIndex, position);
					}
					else if (!regenChunk && !isChunkAtPosition(glm::ivec3(x - 1, y, z)))
					{
						regenChunk = true;
					}

					if (isChunkAtPosition(glm::ivec3(x + 1, y, z)) && !isCubeAtPosition(glm::ivec3(x + 1, y, z)))
					{
						addCubeFace(vertexArray.m_vertexBuffer, texture, chunk.getCubeDetails(position), eCubeSide::Right, 
							opaqueElementBufferIndex, transparentElementBufferIndex, position);
					}
					else if (!regenChunk && !isChunkAtPosition(glm::ivec3(x + 1, y, z)))
					{
						regenChunk = true;
					}

					if (!isCubeAtPosition(glm::ivec3(x, y + 1, z)))
					{
						addCubeFace(vertexArray.m_vertexBuffer, texture, chunk.getCubeDetails(position), eCubeSide::Top, 
							opaqueElementBufferIndex, transparentElementBufferIndex, position);
					}

					if (isChunkAtPosition(glm::ivec3(x, y, z - 1)) && !isCubeAtPosition(glm::ivec3(x, y, z - 1)))
					{
						addCubeFace(vertexArray.m_vertexBuffer, texture, chunk.getCubeDetails(position), eCubeSide::Back, 
							opaqueElementBufferIndex, transparentElementBufferIndex, position);
					}
					else if (!regenChunk && !isChunkAtPosition(glm::ivec3(x, y, z - 1)))
					{
						regenChunk = true;
					}

					if (isChunkAtPosition(glm::ivec3(x, y, z + 1)) && !isCubeAtPosition(glm::ivec3(x, y, z + 1)))
					{
						addCubeFace(vertexArray.m_vertexBuffer, texture, chunk.getCubeDetails(position), eCubeSide::Front, 
							opaqueElementBufferIndex, transparentElementBufferIndex, position);
					}
					else if (!regenChunk && !isChunkAtPosition(glm::ivec3(x, y, z + 1)))
					{
						regenChunk = true;
					}*/
				}
			}
		}
	}

	vertexArray.m_attachOpaqueVBO = true;
	vertexArray.m_attachTransparentVBO = true;

	//if (regenChunk)
	//{
	//	m_chunkMeshRegenerateQueue.push_back(chunkStartingPosition);
	//}
	//else
	//{

	//}
}

void ChunkManager::deleteChunks(const Rectangle& visibilityRect, std::unordered_map<glm::ivec3, VertexArray>& VAOs)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	for (auto chunk = m_chunks.begin(); chunk != m_chunks.end();)
	{
		Rectangle chunkAABB(glm::ivec2(chunk->second.getStartingPosition().x, chunk->second.getStartingPosition().z) +
			glm::ivec2(Utilities::CHUNK_WIDTH / 2.0f, Utilities::CHUNK_DEPTH / 2.0f), 16);
		if (!visibilityRect.contains(chunkAABB))
		{
			glm::ivec3 chunkStartingPosition = chunk->second.getStartingPosition();
			auto VAO = VAOs.find(chunkStartingPosition);
			assert(VAO != VAOs.end());
			if (VAO != VAOs.end())
			{
				VAO->second.m_destroy = true;
			}

			auto chunkToRegen = std::find_if(m_chunkMeshRegenerateQueue.begin(), m_chunkMeshRegenerateQueue.end(), [chunkStartingPosition](const auto& position)
			{
				return position == chunkStartingPosition;
			});
			if (chunkToRegen != m_chunkMeshRegenerateQueue.end())
			{
				m_chunkMeshRegenerateQueue.erase(chunkToRegen);
			}

			chunk = m_chunks.erase(chunk);
		}
		else
		{
			++chunk;
		}
	}
}

void ChunkManager::addChunks(const Rectangle& visibilityRect, std::unordered_map<glm::ivec3, VertexArray>& VAOs, glm::vec3 playerPosition, const Texture& texture)
{
	std::queue<const Chunk*> newlyAddedChunks;
	glm::ivec3 startPosition = Utilities::getClosestChunkStartingPosition(playerPosition);	
	for (int z = startPosition.z - Utilities::VISIBILITY_DISTANCE; z < startPosition.z + Utilities::VISIBILITY_DISTANCE; z += Utilities::CHUNK_DEPTH)
	{
		for (int x = startPosition.x - Utilities::VISIBILITY_DISTANCE; x < startPosition.x + Utilities::VISIBILITY_DISTANCE; x += Utilities::CHUNK_WIDTH)
		{
			glm::ivec3 position(x, 0, z);
			if (m_chunks.find(position) == m_chunks.cend())
			{
				auto newChunk = m_chunks.emplace(std::piecewise_construct,
					std::forward_as_tuple(position),
					std::forward_as_tuple(position)).first;

				newlyAddedChunks.push(&newChunk->second);
			}
		}
	}

	std::lock_guard<std::mutex> lock(m_mutex);
	while (!newlyAddedChunks.empty())
	{
		const Chunk* newChunk = newlyAddedChunks.front();
		newlyAddedChunks.pop();

		auto newVAO = VAOs.emplace(std::piecewise_construct,
			std::forward_as_tuple(newChunk->getStartingPosition()),
			std::forward_as_tuple()).first;

		generateChunkMesh(newVAO->second, texture, *newChunk);
	}
}

void ChunkManager::regenChunks(const Rectangle& visibilityRect, std::unordered_map<glm::ivec3, VertexArray>& VAOs, glm::vec3 playerPosition, const Texture& texture)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	for (auto chunkStartingPosition = m_chunkMeshRegenerateQueue.begin(); chunkStartingPosition != m_chunkMeshRegenerateQueue.end();)
	{
		auto chunk = m_chunks.find(*chunkStartingPosition);
		if (chunk != m_chunks.cend())
		{
			if (m_chunks.find(glm::ivec3(chunkStartingPosition->x - Utilities::CHUNK_WIDTH, 0, chunkStartingPosition->z)) != m_chunks.cend() &&
				m_chunks.find(glm::ivec3(chunkStartingPosition->x + Utilities::CHUNK_WIDTH, 0, chunkStartingPosition->z)) != m_chunks.cend() &&
				m_chunks.find(glm::ivec3(chunkStartingPosition->x, 0, chunkStartingPosition->z - Utilities::CHUNK_DEPTH)) != m_chunks.cend() &&
				m_chunks.find(glm::ivec3(chunkStartingPosition->x, 0, chunkStartingPosition->z + Utilities::CHUNK_DEPTH)) != m_chunks.cend())
			{
				auto VAO = VAOs.find(*chunkStartingPosition);
				assert(VAO != VAOs.end());
				if (VAO != VAOs.end())
				{
					VAO->second.reset();
					generateChunkMesh(VAO->second, texture, chunk->second);
				}

				chunkStartingPosition = m_chunkMeshRegenerateQueue.erase(chunkStartingPosition);
			}
			else
			{
				++chunkStartingPosition;
			}
		}
		else
		{
			chunkStartingPosition = m_chunkMeshRegenerateQueue.erase(chunkStartingPosition);
		}
	}
}

std::array<const Chunk*, static_cast<size_t>(eDirection::Total)> ChunkManager::getNeighbouringChunks(glm::ivec3 chunkStartingPosition) const
{
	std::array<const Chunk*, static_cast<size_t>(eDirection::Total)> neighbouringChunks;

	glm::ivec3 leftChunkStartingPosition(chunkStartingPosition.x - Utilities::CHUNK_WIDTH, chunkStartingPosition.y, chunkStartingPosition.z);
	auto leftChunk = m_chunks.find(leftChunkStartingPosition);
	if (leftChunk != m_chunks.cend())
	{
		neighbouringChunks[static_cast<int>(eDirection::Left)] = &leftChunk->second;
	}

	glm::ivec3 rightChunkStartingPosition(chunkStartingPosition.x + Utilities::CHUNK_WIDTH, chunkStartingPosition.y, chunkStartingPosition.z);
	auto rightChunk = m_chunks.find(leftChunkStartingPosition);
	if (rightChunk != m_chunks.cend())
	{
		neighbouringChunks[static_cast<int>(eDirection::Left)] = &rightChunk->second;
	}

	glm::ivec3 forwardChunkStartingPosition(chunkStartingPosition.x, chunkStartingPosition.y, chunkStartingPosition.z + Utilities::CHUNK_DEPTH);
	auto forwardChunk = m_chunks.find(leftChunkStartingPosition);
	if (forwardChunk != m_chunks.cend())
	{
		neighbouringChunks[static_cast<int>(eDirection::Left)] = &forwardChunk->second;
	}
	
}

const Chunk* ChunkManager::getNeighbouringChunkAtPosition(glm::ivec3 chunkStartingPosition) const
{
	auto chunk = m_chunks.find(chunkStartingPosition);
	return (chunk != m_chunks.cend() ? &chunk->second : nullptr);
}
