#include "ChunkGenerator.h"
#include "Utilities.h"
#include "VertexBuffer.h"
#include "VertexArray.h"
#include "CubeID.h"
#include "Rectangle.h"
#include "Camera.h"
#include <iostream>

ChunkGenerator::Regenerate::Regenerate(const Chunk& chunk, VertexArrayFromPool&& vertexArrayFromPool)
	: vertexArrayToRegenerate(std::move(vertexArrayFromPool)),
	chunkToRegenerate(chunk),
	regenerated(false)
{}

ChunkGenerator::ChunkGenerator(const glm::ivec3& playerPosition)
	: m_chunkPool(),
	m_vertexArrayPool(),
	m_VAOs(),
	m_chunks()
{
	//Generate Chunks
	for (int z = playerPosition.z - Utilities::VISIBILITY_DISTANCE; z <= playerPosition.z + Utilities::VISIBILITY_DISTANCE; z += Utilities::CHUNK_DEPTH)
	{
		for (int x = playerPosition.x - Utilities::VISIBILITY_DISTANCE; x <= playerPosition.x + Utilities::VISIBILITY_DISTANCE; x += Utilities::CHUNK_WIDTH)
		{
			glm::ivec3 chunkStartingPosition(x, 0, z);
			Utilities::getClosestChunkStartingPosition(chunkStartingPosition);

			ChunkFromPool chunkFromPool(m_chunkPool, chunkStartingPosition);
			if (chunkFromPool.object)
			{
				m_chunks.emplace(std::piecewise_construct,
					std::forward_as_tuple(chunkStartingPosition),
					std::forward_as_tuple(std::move(chunkFromPool)));
			}
			else
			{
				std::cout << "Unable to retrieve Chunk not in use.\n";
			}
		}
	}

	//Generate VAOs
	for (const auto& chunk : m_chunks)
	{
		VertexArrayFromPool VAOFromPool(m_vertexArrayPool);
		if (VAOFromPool.object)
		{
			generateChunkMesh(*VAOFromPool.object, *chunk.second.object);

			if (VAOFromPool.object->m_awaitingRegeneration)
			{
				m_regenerate.emplace(std::piecewise_construct,
					std::forward_as_tuple(chunk.first),
					std::forward_as_tuple(*chunk.second.object, std::move(VAOFromPool)));
			}
			else
			{
				m_VAOs.emplace(std::piecewise_construct,
					std::forward_as_tuple(chunk.first),
					std::forward_as_tuple(std::move(VAOFromPool)));
			}
		}
		else
		{ 
			std::cout << "Unable to retrieve VAO not in use \n";
		}
	}
}

void ChunkGenerator::update(const glm::vec3& cameraPosition, const sf::Window& window, std::atomic<bool>& resetGame, 
	std::mutex& cameraMutex, std::mutex& renderingMutex)	
{
	while (!resetGame)
	{
		std::unique_lock<std::mutex> cameraLock(cameraMutex);
		glm::ivec3 position = cameraPosition;
		cameraLock.unlock();

		deleteChunks(position, renderingMutex);
		addChunks(position);
		handleRegeneration(renderingMutex);
	}
}

void ChunkGenerator::renderOpaque() const
{
	for (const auto& VAO : m_VAOs)
	{
		if (VAO.second.object->m_opaqueVertexBuffer.bindToVAO)
		{
			VAO.second.object->attachOpaqueVBO();
		}

		if (VAO.second.object->m_opaqueVertexBuffer.displayable)
		{
			VAO.second.object->bindOpaqueVAO();
			glDrawElements(GL_TRIANGLES, VAO.second.object->m_opaqueVertexBuffer.indicies.size(), GL_UNSIGNED_INT, nullptr);
		}
	}
}

void ChunkGenerator::renderTransparent() const
{
	for (const auto& VAO : m_VAOs)
	{
		if (VAO.second.object->m_transparentVertexBuffer.bindToVAO)
		{
			VAO.second.object->attachTransparentVBO();
		}

		if (VAO.second.object->m_transparentVertexBuffer.displayable)
		{
			VAO.second.object->bindTransparentVAO();
			glDrawElements(GL_TRIANGLES, VAO.second.object->m_transparentVertexBuffer.indicies.size(), GL_UNSIGNED_INT, nullptr);
		}
	}
}

void ChunkGenerator::addCubeFace(VertexBuffer& vertexBuffer, eCubeType cubeType, eCubeSide cubeSide, const glm::ivec3& cubePosition, bool transparent)
{
	glm::ivec3 position = cubePosition;
	switch (cubeSide)
	{
	case eCubeSide::Front:
		for (const glm::ivec3& i : Utilities::CUBE_FACE_FRONT)
		{
			position += i;
			vertexBuffer.positions.emplace_back(position.x, position.y, position.z);
			position = cubePosition;
		
			if (transparent)
			{
				vertexBuffer.lightIntensityVertices.push_back(Utilities::DEFAULT_LIGHTING_INTENSITY);
			}
			else
			{
				vertexBuffer.lightIntensityVertices.push_back(Utilities::FRONT_FACE_LIGHTING_INTENSITY);
			}
		}

		Utilities::getTextCoords(vertexBuffer.textCoords, cubeSide, cubeType);
		break;
	case eCubeSide::Back:
		for (const glm::ivec3& i : Utilities::CUBE_FACE_BACK)
		{
			position += i;
			vertexBuffer.positions.emplace_back(position.x, position.y, position.z);
			position = cubePosition;
			
			if (transparent)
			{
				vertexBuffer.lightIntensityVertices.push_back(Utilities::DEFAULT_LIGHTING_INTENSITY);
			}
			else
			{
				vertexBuffer.lightIntensityVertices.push_back(Utilities::BACK_FACE_LIGHTING_INTENSITY);
			}
		}

		Utilities::getTextCoords(vertexBuffer.textCoords, cubeSide, cubeType);

		break;
	case eCubeSide::Left:
		for (const glm::ivec3& i : Utilities::CUBE_FACE_LEFT)
		{
			position += i;
			vertexBuffer.positions.emplace_back(position.x, position.y, position.z);
			position = cubePosition;

			if (transparent)
			{
				vertexBuffer.lightIntensityVertices.push_back(Utilities::DEFAULT_LIGHTING_INTENSITY);
			}
			else
			{
				vertexBuffer.lightIntensityVertices.push_back(Utilities::LEFT_FACE_LIGHTING_INTENSITY);
			}
		}
		Utilities::getTextCoords(vertexBuffer.textCoords, cubeSide, cubeType);
		break;
	case eCubeSide::Right:
		for (const glm::ivec3& i : Utilities::CUBE_FACE_RIGHT)
		{
			position += i;
			vertexBuffer.positions.emplace_back(position.x, position.y, position.z);
			position = cubePosition;

			if (transparent)
			{
				vertexBuffer.lightIntensityVertices.push_back(Utilities::DEFAULT_LIGHTING_INTENSITY);
			}
			else
			{
				vertexBuffer.lightIntensityVertices.push_back(Utilities::RIGHT_FACE_LIGHTING_INTENSITY);
			}
		}

		Utilities::getTextCoords(vertexBuffer.textCoords, cubeSide, cubeType);

		break;
	case eCubeSide::Top:
		for (const glm::ivec3& i : Utilities::CUBE_FACE_TOP)
		{
			position += i;
			vertexBuffer.positions.emplace_back(position.x, position.y, position.z);
			position = cubePosition;

			if (transparent)
			{
				vertexBuffer.lightIntensityVertices.push_back(Utilities::DEFAULT_LIGHTING_INTENSITY);
			}
			else
			{
				vertexBuffer.lightIntensityVertices.push_back(Utilities::TOP_LIGHTING_INTENSITY);
			}
			
		}
		Utilities::getTextCoords(vertexBuffer.textCoords, cubeSide, cubeType);

		break;
	case eCubeSide::Bottom:
		for (const glm::ivec3& i : Utilities::CUBE_FACE_BOTTOM)
		{
			position += i;
			vertexBuffer.positions.emplace_back(position.x, position.y, position.z);
			position = cubePosition;

			if (transparent)
			{
				vertexBuffer.lightIntensityVertices.push_back(Utilities::DEFAULT_LIGHTING_INTENSITY);
			}
			else
			{
				vertexBuffer.lightIntensityVertices.push_back(Utilities::BOTTOM_FACE_LIGHTING_INTENSITY);
			}
		}

		Utilities::getTextCoords(vertexBuffer.textCoords, cubeSide, cubeType);

		break;
	}

	for (unsigned int i : Utilities::CUBE_FACE_INDICIES)
	{
		vertexBuffer.indicies.emplace_back(i + vertexBuffer.elementBufferIndex);
	}

	vertexBuffer.elementBufferIndex += Utilities::CUBE_FACE_INDICIE_COUNT;
}

bool ChunkGenerator::isCubeAtPosition(const glm::ivec3& position, const Chunk& chunk) const
{
	char cubeType = chunk.getCubeDetailsWithoutBoundsCheck(position);
	return (cubeType != static_cast<char>(eCubeType::Invalid) && cubeType != static_cast<char>(eCubeType::Water) ? true : false);
}

void ChunkGenerator::generateChunkMesh(VertexArray& vertexArray, const Chunk& chunk)
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
						addCubeFace(vertexArray.m_transparentVertexBuffer, cubeType, eCubeSide::Top, position, true);
					}
				}
				else if (cubeType == eCubeType::Leaves)
				{
					//Left Face
					glm::ivec3 leftPosition(x - 1, y, z);
					if (chunk.isPositionInBounds(leftPosition))
					{
						if (!isCubeAtPosition(leftPosition, chunk))
						{
							addCubeFace(vertexArray.m_transparentVertexBuffer, cubeType, eCubeSide::Left, position, true);
						}
					}
					else if (leftNeighbouringChunk &&
						!isCubeAtPosition(leftPosition, *leftNeighbouringChunk))
					{
						addCubeFace(vertexArray.m_transparentVertexBuffer, cubeType, eCubeSide::Left, position, true);
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
							addCubeFace(vertexArray.m_transparentVertexBuffer, cubeType, eCubeSide::Right, position, true);
						}
					}
					else if (rightNeighbouringChunk &&
						!isCubeAtPosition(rightPosition, *rightNeighbouringChunk))
					{
						addCubeFace(vertexArray.m_transparentVertexBuffer, cubeType, eCubeSide::Right, position, true);
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
							addCubeFace(vertexArray.m_transparentVertexBuffer, cubeType, eCubeSide::Front, position, true);
						}
					}
					else if (forwardNeighbouringChunk &&
						!isCubeAtPosition(forwardPosition, *forwardNeighbouringChunk))
					{
						addCubeFace(vertexArray.m_transparentVertexBuffer, cubeType, eCubeSide::Front, position, true);
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
							addCubeFace(vertexArray.m_transparentVertexBuffer, cubeType, eCubeSide::Back, position, true);
						}
					}
					else if (backNeighbouringChunk &&
						!isCubeAtPosition(backPosition, *backNeighbouringChunk))
					{
						addCubeFace(vertexArray.m_transparentVertexBuffer, cubeType, eCubeSide::Back, position, true);
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
							addCubeFace(vertexArray.m_transparentVertexBuffer, cubeType, eCubeSide::Top, position, true);
						}
					}

					//Bottom Face
					addCubeFace(vertexArray.m_transparentVertexBuffer, cubeType, eCubeSide::Bottom, position, true);
				}
				else
				{
					//Left Face
					glm::ivec3 leftPosition(x - 1, y, z);
					if (chunk.isPositionInBounds(leftPosition))
					{
						if (!isCubeAtPosition(leftPosition, chunk))
						{
							addCubeFace(vertexArray.m_opaqueVertexBuffer, cubeType, eCubeSide::Left, position, false);
						}
					}
					else if (leftNeighbouringChunk &&
						!isCubeAtPosition(leftPosition, *leftNeighbouringChunk))
					{
						addCubeFace(vertexArray.m_opaqueVertexBuffer, cubeType, eCubeSide::Left, position, false);
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
							addCubeFace(vertexArray.m_opaqueVertexBuffer, cubeType, eCubeSide::Right, position, false);
						}
					}
					else if (rightNeighbouringChunk &&
						!isCubeAtPosition(rightPosition, *rightNeighbouringChunk))
					{
						addCubeFace(vertexArray.m_opaqueVertexBuffer, cubeType, eCubeSide::Right, position, false);
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
							addCubeFace(vertexArray.m_opaqueVertexBuffer, cubeType, eCubeSide::Front, position, false);
						}
					}
					else if (forwardNeighbouringChunk &&
						!isCubeAtPosition(forwardPosition, *forwardNeighbouringChunk))
					{
						addCubeFace(vertexArray.m_opaqueVertexBuffer, cubeType, eCubeSide::Front, position, false);
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
							addCubeFace(vertexArray.m_opaqueVertexBuffer, cubeType, eCubeSide::Back, position, false);
						}
					}
					else if (backNeighbouringChunk &&
						!isCubeAtPosition(backPosition, *backNeighbouringChunk))
					{
						addCubeFace(vertexArray.m_opaqueVertexBuffer, cubeType, eCubeSide::Back, position, false);
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
							addCubeFace(vertexArray.m_opaqueVertexBuffer, cubeType, eCubeSide::Top, position, false);
						}
					}
				}
			}
		}
	}

	if(regenChunk)
	{ 
		vertexArray.m_awaitingRegeneration = true;
	}
	else
	{
		if (!vertexArray.m_opaqueVertexBuffer.indicies.empty())
		{
			vertexArray.m_opaqueVertexBuffer.bindToVAO = true;
		}

		if (!vertexArray.m_transparentVertexBuffer.indicies.empty())
		{
			vertexArray.m_transparentVertexBuffer.bindToVAO = true;
		}

		vertexArray.m_awaitingRegeneration = false;
	}
}

void ChunkGenerator::deleteChunks(const glm::ivec3& playerPosition, std::mutex& renderingMutex)
{
	glm::ivec3 startingPosition(playerPosition);
	Utilities::getClosestMiddlePosition(startingPosition);
	Rectangle visibilityRect(glm::vec2(startingPosition.x, startingPosition.z), Utilities::VISIBILITY_DISTANCE);

	std::lock_guard<std::mutex> renderingLock(renderingMutex);
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

void ChunkGenerator::addChunks(const glm::vec3& playerPosition)
{
	std::vector<const Chunk*> addedChunks;
	glm::ivec3 startPosition(playerPosition);
	Utilities::getClosestMiddlePosition(startPosition);
	for (int z = startPosition.z - Utilities::VISIBILITY_DISTANCE; z <= startPosition.z + Utilities::VISIBILITY_DISTANCE; z += Utilities::CHUNK_DEPTH)
	{
		for (int x = startPosition.x - Utilities::VISIBILITY_DISTANCE; x <= startPosition.x + Utilities::VISIBILITY_DISTANCE; x += Utilities::CHUNK_WIDTH)
		{
			glm::ivec3 chunkStartingPosition(x, 0, z);
			Utilities::getClosestChunkStartingPosition(chunkStartingPosition);
			if (m_chunks.find(chunkStartingPosition) == m_chunks.cend())
			{
				ChunkFromPool chunkFromPool(m_chunkPool, chunkStartingPosition);
				if (chunkFromPool.object)
				{
					const Chunk* chunk = m_chunks.emplace(std::piecewise_construct,
						std::forward_as_tuple(chunkStartingPosition),
						std::forward_as_tuple(std::move(chunkFromPool))).first->second.object;

					addedChunks.push_back(chunk);
				}
				else
				{
					std::cout << "Unable to retrieve Chunk not in use\n";
				}
			}
		}
	}

	for (const auto& addedChunk : addedChunks)
	{
		VertexArrayFromPool VAOFromPool(m_vertexArrayPool);
		if (VAOFromPool.object)
		{
			VertexArray& VAO = *m_regenerate.emplace(std::piecewise_construct,
				std::forward_as_tuple(addedChunk->getStartingPosition()),
				std::forward_as_tuple(*addedChunk, std::move(VAOFromPool))).first->second.vertexArrayToRegenerate.object;

			generateChunkMesh(VAO, *addedChunk);
		}
		else
		{
			std::cout << "Unable to retrieve VAO not in use\n";
		}
	}
}

void ChunkGenerator::handleRegeneration(std::mutex& renderingMutex)
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

const Chunk* ChunkGenerator::getNeighbouringChunkAtPosition(const glm::ivec3& chunkStartingPosition, eDirection direction) const
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

