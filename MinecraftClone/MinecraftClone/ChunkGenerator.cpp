#include "ChunkGenerator.h"
#include "Utilities.h"
#include "VertexBuffer.h"
#include "VertexArray.h"
#include "CubeID.h"
#include "Rectangle.h"
#include "Camera.h"
#include "Frustum.h"
#include <iostream>

namespace
{
	constexpr float DEFAULT_LIGHTING_INTENSITY = 1.0f;
	constexpr float TOP_LIGHTING_INTENSITY = 1.35f;
	constexpr float FRONT_FACE_LIGHTING_INTENSITY = 1.2f;
	constexpr float BACK_FACE_LIGHTING_INTENSITY = 1.2f;
	constexpr float LEFT_FACE_LIGHTING_INTENSITY = 1.1f;
	constexpr float RIGHT_FACE_LIGHTING_INTENSITY = 1.1f;
	constexpr float BOTTOM_FACE_LIGHTING_INTENSITY = 0.4f;
	constexpr int CUBE_FACE_INDICIE_COUNT = 4;

	constexpr std::array<unsigned int, 6> CUBE_FACE_INDICIES =
	{
		0, 1, 2,
		2, 3, 0
	};

	constexpr std::array<glm::vec2, 4> TEXT_COORDS =
	{
		glm::vec2(0.0f, 0.0f),
		glm::vec2(1.0f, 0.0f),
		glm::vec2(1.0f, 1.0f),
		glm::vec2(0.0f, 1.0f)
	};

	void getTextCoords(std::vector<glm::vec3>& textCoords, eCubeSide cubeSide, eCubeType cubeType)
	{
		int textureLayer = 0;
		switch (cubeType)
		{
		case eCubeType::Stone:
			textureLayer = static_cast<int>(eTextureLayer::Stone);
			break;
		case eCubeType::Sand:
			textureLayer = static_cast<int>(eTextureLayer::Sand);
			break;
		case eCubeType::Grass:
			switch (cubeSide)
			{
			case eCubeSide::Back:
			case eCubeSide::Front:
			case eCubeSide::Left:
			case eCubeSide::Right:
				textureLayer = static_cast<int>(eTextureLayer::GrassSide);
				break;
			case eCubeSide::Top:
				textureLayer = static_cast<int>(eTextureLayer::Grass);
			}
			break;
		case eCubeType::Log:
			textureLayer = static_cast<int>(eTextureLayer::Log);
			break;
		case eCubeType::LogTop:
			switch (cubeSide)
			{
			case eCubeSide::Back:
			case eCubeSide::Front:
			case eCubeSide::Left:
			case eCubeSide::Right:
				textureLayer = static_cast<int>(eTextureLayer::Log);
				break;
			case eCubeSide::Top:
				textureLayer = static_cast<int>(eTextureLayer::LogTop);
				break;
			}
			break;
		case eCubeType::Leaves:
			textureLayer = static_cast<int>(eTextureLayer::Leaves);
			break;
		case eCubeType::Cactus:
			textureLayer = static_cast<int>(eTextureLayer::Cactus);
			break;
		case eCubeType::CactusTop:
			switch (cubeSide)
			{
			case eCubeSide::Back:
			case eCubeSide::Front:
			case eCubeSide::Left:
			case eCubeSide::Right:
				textureLayer = static_cast<int>(eTextureLayer::Cactus);
				break;
			case eCubeSide::Top:
				textureLayer = static_cast<int>(eTextureLayer::CactusTop);
				break;
			}
			break;
		case eCubeType::Water:
			textureLayer = static_cast<int>(eTextureLayer::Water);
			break;
		case eCubeType::Shrub:
			textureLayer = static_cast<int>(eTextureLayer::Shrub);
			break;
		case eCubeType::TallGrass:
			textureLayer = static_cast<int>(eTextureLayer::TallGrass);
			break;

		default:
			textureLayer = static_cast<int>(eTextureLayer::Error);
		}

		assert(textureLayer != static_cast<int>(eTextureLayer::Error));
		for (const auto& i : TEXT_COORDS)
		{
			textCoords.emplace_back(i.x, i.y, textureLayer);
		}
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

	constexpr std::array<glm::ivec3, 4> CUBE_FACE_FRONT = { glm::ivec3(0, 0, 1), glm::ivec3(1, 0, 1), glm::ivec3(1, 1, 1), glm::ivec3(0, 1, 1) };
	constexpr std::array<glm::ivec3, 4> CUBE_FACE_BACK = { glm::ivec3(1, 0, 0), glm::ivec3(0, 0, 0), glm::ivec3(0, 1, 0), glm::ivec3(1, 1, 0) };

	constexpr std::array<glm::ivec3, 4> CUBE_FACE_LEFT = { glm::ivec3(0, 0, 0), glm::ivec3(0, 0, 1), glm::ivec3(0, 1, 1), glm::ivec3(0, 1, 0) };
	constexpr std::array<glm::ivec3, 4> CUBE_FACE_RIGHT = { glm::ivec3(1, 0, 1), glm::ivec3(1, 0, 0), glm::ivec3(1, 1, 0), glm::ivec3(1, 1, 1) };

	constexpr std::array<glm::ivec3, 4> CUBE_FACE_TOP = { glm::ivec3(0, 1, 1), glm::ivec3(1, 1, 1), glm::ivec3(1, 1, 0), glm::ivec3(0, 1, 0) };
	constexpr std::array<glm::ivec3, 4> CUBE_FACE_BOTTOM = { glm::ivec3(0, 0, 0), glm::ivec3(1, 0, 0), glm::ivec3(1, 0, 1), glm::ivec3(0, 0, 1) };

	constexpr std::array<glm::ivec3, 4> FIRST_DIAGONAL_FACE = { glm::ivec3(0, 0, 0), glm::ivec3(1, 0, 1), glm::ivec3(1, 1, 1), glm::ivec3(0, 1, 0) };
	constexpr std::array<glm::ivec3, 4> SECOND_DIAGONAL_FACE = { glm::ivec3(0, 0, 1), glm::ivec3(1, 0, 0), glm::ivec3(1, 1, 0), glm::ivec3(0, 1, 1) };
}

ChunkGenerator::Regenerate::Regenerate(const ObjectFromPool<Chunk>& chunkFromPool, ObjectFromPool<VertexArray>&& vertexArrayFromPool)
	: vertexArrayToRegenerate(std::move(vertexArrayFromPool)),
	chunkFromPool(chunkFromPool),
	regenerated(false)
{}

ChunkGenerator::ChunkGenerator(const glm::ivec3& playerPosition)
	: m_chunkPool(Utilities::VISIBILITY_DISTANCE, Utilities::CHUNK_WIDTH, Utilities::CHUNK_DEPTH),
	m_vertexArrayPool(Utilities::VISIBILITY_DISTANCE, Utilities::CHUNK_WIDTH, Utilities::CHUNK_DEPTH),
	m_VAOs(),
	m_chunks()
{
	//Generate Chunks
	for (int z = playerPosition.z - Utilities::VISIBILITY_DISTANCE; z <= playerPosition.z + Utilities::VISIBILITY_DISTANCE; z += Utilities::CHUNK_DEPTH)
	{
		for (int x = playerPosition.x - Utilities::VISIBILITY_DISTANCE; x <= playerPosition.x + Utilities::VISIBILITY_DISTANCE; x += Utilities::CHUNK_WIDTH)
		{
			glm::ivec3 chunkStartingPosition(x, 0, z);
			getClosestChunkStartingPosition(chunkStartingPosition);

			ObjectFromPool<Chunk> chunkFromPool = m_chunkPool.getNextAvailableObject();
			assert(chunkFromPool.getObject());
			if (chunkFromPool.getObject())
			{
				chunkFromPool.getObject()->reuse(chunkStartingPosition);
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
		ObjectFromPool<VertexArray> VAOFromPool = m_vertexArrayPool.getNextAvailableObject();
		assert(VAOFromPool.getObject());
		if (VAOFromPool.getObject())
		{
			generateChunkMesh(*VAOFromPool.getObject(), *chunk.second.getObject());

			if (VAOFromPool.getObject()->m_regenerate)
			{
				m_regenerate.emplace(std::piecewise_construct,
					std::forward_as_tuple(chunk.first),
					std::forward_as_tuple(chunk.second, std::move(VAOFromPool)));
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

void ChunkGenerator::removeCubeAtPosition(const glm::ivec3& position)
{
	glm::ivec3 chunkStartingPosition;
	getClosestChunkStartingPosition(chunkStartingPosition);
	auto chunk = m_chunks.find(chunkStartingPosition);
	assert(chunk != m_chunks.end());
	if (chunk != m_chunks.end())
	{

	}
}

void ChunkGenerator::update(const glm::vec3& cameraPosition, const sf::Window& window, std::atomic<bool>& resetGame, 
	std::mutex& cameraMutex, std::mutex& renderingMutex)	
{
	while (!resetGame && window.isOpen())
	{
		std::unique_lock<std::mutex> cameraLock(cameraMutex);
		glm::ivec3 position = cameraPosition;
		cameraLock.unlock();

		deleteChunks(position, renderingMutex);
		addChunks(position);
		handleRegeneration(renderingMutex);
	}
}

void ChunkGenerator::renderOpaque(const Frustum& frustum) const
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

void ChunkGenerator::renderTransparent(const Frustum& frustum) const
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

void ChunkGenerator::addCubeFace(VertexBuffer& vertexBuffer, eCubeType cubeType, eCubeSide cubeSide, const glm::ivec3& cubePosition, bool transparent)
{
	glm::ivec3 position = cubePosition;
	switch (cubeSide)
	{
	case eCubeSide::Front:
		for (const glm::ivec3& i : CUBE_FACE_FRONT)
		{
			position += i;
			vertexBuffer.positions.emplace_back(position.x, position.y, position.z);
			position = cubePosition;
		
			if (transparent)
			{
				vertexBuffer.lightIntensityVertices.push_back(DEFAULT_LIGHTING_INTENSITY);
			}
			else
			{
				vertexBuffer.lightIntensityVertices.push_back(FRONT_FACE_LIGHTING_INTENSITY);
			}
		}

		getTextCoords(vertexBuffer.textCoords, cubeSide, cubeType);
		break;
	case eCubeSide::Back:
		for (const glm::ivec3& i : CUBE_FACE_BACK)
		{
			position += i;
			vertexBuffer.positions.emplace_back(position.x, position.y, position.z);
			position = cubePosition;
			
			if (transparent)
			{
				vertexBuffer.lightIntensityVertices.push_back(DEFAULT_LIGHTING_INTENSITY);
			}
			else
			{
				vertexBuffer.lightIntensityVertices.push_back(BACK_FACE_LIGHTING_INTENSITY);
			}
		}

		getTextCoords(vertexBuffer.textCoords, cubeSide, cubeType);

		break;
	case eCubeSide::Left:
		for (const glm::ivec3& i : CUBE_FACE_LEFT)
		{
			position += i;
			vertexBuffer.positions.emplace_back(position.x, position.y, position.z);
			position = cubePosition;

			if (transparent)
			{
				vertexBuffer.lightIntensityVertices.push_back(DEFAULT_LIGHTING_INTENSITY);
			}
			else
			{
				vertexBuffer.lightIntensityVertices.push_back(LEFT_FACE_LIGHTING_INTENSITY);
			}
		}
		getTextCoords(vertexBuffer.textCoords, cubeSide, cubeType);
		break;
	case eCubeSide::Right:
		for (const glm::ivec3& i : CUBE_FACE_RIGHT)
		{
			position += i;
			vertexBuffer.positions.emplace_back(position.x, position.y, position.z);
			position = cubePosition;

			if (transparent)
			{
				vertexBuffer.lightIntensityVertices.push_back(DEFAULT_LIGHTING_INTENSITY);
			}
			else
			{
				vertexBuffer.lightIntensityVertices.push_back(RIGHT_FACE_LIGHTING_INTENSITY);
			}
		}

		getTextCoords(vertexBuffer.textCoords, cubeSide, cubeType);

		break;
	case eCubeSide::Top:
		for (const glm::ivec3& i : CUBE_FACE_TOP)
		{
			position += i;
			vertexBuffer.positions.emplace_back(position.x, position.y, position.z);
			position = cubePosition;

			if (transparent)
			{
				vertexBuffer.lightIntensityVertices.push_back(DEFAULT_LIGHTING_INTENSITY);
			}
			else
			{
				vertexBuffer.lightIntensityVertices.push_back(TOP_LIGHTING_INTENSITY);
			}
			
		}
		getTextCoords(vertexBuffer.textCoords, cubeSide, cubeType);

		break;
	case eCubeSide::Bottom:
		for (const glm::ivec3& i : CUBE_FACE_BOTTOM)
		{
			position += i;
			vertexBuffer.positions.emplace_back(position.x, position.y, position.z);
			position = cubePosition;

			if (transparent)
			{
				vertexBuffer.lightIntensityVertices.push_back(DEFAULT_LIGHTING_INTENSITY);
			}
			else
			{
				vertexBuffer.lightIntensityVertices.push_back(BOTTOM_FACE_LIGHTING_INTENSITY);
			}
		}

		getTextCoords(vertexBuffer.textCoords, cubeSide, cubeType);

		break;
	}

	for (unsigned int i : CUBE_FACE_INDICIES)
	{
		vertexBuffer.indicies.emplace_back(i + vertexBuffer.elementBufferIndex);
	}

	vertexBuffer.elementBufferIndex += CUBE_FACE_INDICIE_COUNT;
}

void ChunkGenerator::addDiagonalCubeFace(VertexBuffer& vertexBuffer, eCubeType cubeType, const glm::ivec3& cubePosition, 
	const std::array<glm::ivec3, 4>& diagonalFace)
{
	//Positions
	glm::ivec3 position = cubePosition;
	for (const glm::ivec3& i : diagonalFace)
	{
		position += i;
		vertexBuffer.positions.emplace_back(position.x, position.y, position.z);
		position = cubePosition;

		//Lighting
		vertexBuffer.lightIntensityVertices.push_back(DEFAULT_LIGHTING_INTENSITY);
	}

	//Texture Coordinates
	getTextCoords(vertexBuffer.textCoords, eCubeSide::Front, cubeType);
	
	//Indicies
	for (unsigned int i : CUBE_FACE_INDICIES)
	{
		vertexBuffer.indicies.emplace_back(i + vertexBuffer.elementBufferIndex);
	}

	vertexBuffer.elementBufferIndex += CUBE_FACE_INDICIE_COUNT;
}

bool ChunkGenerator::isCubeAtPosition(const glm::ivec3& position, const Chunk& chunk) const
{
	char cubeType = chunk.getCubeDetailsWithoutBoundsCheck(position);
	return (cubeType != static_cast<char>(eCubeType::Invalid) && 
		cubeType != static_cast<char>(eCubeType::Water) && 
		cubeType != static_cast<char>(eCubeType::Shrub) &&
		cubeType != static_cast<char>(eCubeType::TallGrass) ? true : false);
}

void ChunkGenerator::generateChunkMesh(VertexArray& vertexArray, const Chunk& chunk)
{
	const glm::ivec3& chunkStartingPosition = chunk.getStartingPosition();
	const glm::ivec3& chunkEndingPosition = chunk.getEndingPosition();
	vertexArray.m_regenerate = false; //Reset regeneration value

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
					if (!vertexArray.m_regenerate)
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
						vertexArray.m_regenerate = true;
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
						vertexArray.m_regenerate = true;
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
						vertexArray.m_regenerate = true;
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
						vertexArray.m_regenerate = true;
					}

					//Top Face
					if (y == Utilities::CHUNK_HEIGHT - 1 || !isCubeAtPosition(glm::ivec3(x, y + 1, z), chunk))
					{
						if (!vertexArray.m_regenerate)
						{
							addCubeFace(vertexArray.m_transparentVertexBuffer, cubeType, eCubeSide::Top, position, true);
						}
					}

					//Bottom Face
					if (!isCubeAtPosition({ x, y - 1, z }, chunk))
					{
						addCubeFace(vertexArray.m_transparentVertexBuffer, cubeType, eCubeSide::Bottom, position, true);
					}
				
				}
				else if (cubeType == eCubeType::Shrub || cubeType == eCubeType::TallGrass)
				{
					addDiagonalCubeFace(vertexArray.m_transparentVertexBuffer, cubeType, position, FIRST_DIAGONAL_FACE);
					addDiagonalCubeFace(vertexArray.m_transparentVertexBuffer, cubeType, position, SECOND_DIAGONAL_FACE);
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
						vertexArray.m_regenerate = true;
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
						vertexArray.m_regenerate = true;
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
						vertexArray.m_regenerate = true;
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
						vertexArray.m_regenerate = true;
					}

					if (cubeType == eCubeType::LogTop)
					{
						addCubeFace(vertexArray.m_opaqueVertexBuffer, cubeType, eCubeSide::Top, position, false);
					}
					else
					{
						//Top Face
						if (y == Utilities::CHUNK_HEIGHT - 1 || !isCubeAtPosition(glm::ivec3(x, y + 1, z), chunk))
						{
							if (!vertexArray.m_regenerate)
							{
								addCubeFace(vertexArray.m_opaqueVertexBuffer, cubeType, eCubeSide::Top, position, false);
							}
						}
					}
				}
			}
		}
	}

	if (!vertexArray.m_regenerate)
	{
		if (!vertexArray.m_opaqueVertexBuffer.indicies.empty())
		{
			vertexArray.m_opaqueVertexBuffer.bindToVAO = true;
		}

		if (!vertexArray.m_transparentVertexBuffer.indicies.empty())
		{
			vertexArray.m_transparentVertexBuffer.bindToVAO = true;
		}
	}
}

void ChunkGenerator::deleteChunks(const glm::ivec3& playerPosition, std::mutex& renderingMutex)
{
	glm::ivec3 startingPosition(playerPosition);
	getClosestMiddlePosition(startingPosition);
	Rectangle visibilityRect(glm::vec2(startingPosition.x, startingPosition.z), Utilities::VISIBILITY_DISTANCE);

	std::lock_guard<std::mutex> renderingLock(renderingMutex);
	//Locate Chunks to delete
	for (auto chunk = m_chunks.begin(); chunk != m_chunks.end();)
	{
		const glm::ivec3& chunkStartingPosition = chunk->second.getObject()->getStartingPosition();
		if (!visibilityRect.contains(chunk->second.getObject()->getAABB()))
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
	std::vector<std::reference_wrapper<ObjectFromPool<Chunk>>> addedChunks;
	glm::ivec3 startPosition(playerPosition);
	getClosestMiddlePosition(startPosition);
	for (int z = startPosition.z - Utilities::VISIBILITY_DISTANCE; z <= startPosition.z + Utilities::VISIBILITY_DISTANCE; z += Utilities::CHUNK_DEPTH)
	{
		for (int x = startPosition.x - Utilities::VISIBILITY_DISTANCE; x <= startPosition.x + Utilities::VISIBILITY_DISTANCE; x += Utilities::CHUNK_WIDTH)
		{
			glm::ivec3 chunkStartingPosition(x, 0, z);
			getClosestChunkStartingPosition(chunkStartingPosition);
			if (m_chunks.find(chunkStartingPosition) == m_chunks.cend())
			{
				ObjectFromPool<Chunk> chunkFromPool = m_chunkPool.getNextAvailableObject();
				assert(chunkFromPool.getObject());
				if (chunkFromPool.getObject())
				{
					chunkFromPool.getObject()->reuse(chunkStartingPosition);

					ObjectFromPool<Chunk>& chunk = m_chunks.emplace(std::piecewise_construct,
						std::forward_as_tuple(chunkStartingPosition),
						std::forward_as_tuple(std::move(chunkFromPool))).first->second;

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
		ObjectFromPool<VertexArray> VAOFromPool = m_vertexArrayPool.getNextAvailableObject();
		assert(VAOFromPool.getObject());
		if (VAOFromPool.getObject())
		{
			VertexArray& VAO = *m_regenerate.emplace(std::piecewise_construct,
				std::forward_as_tuple(addedChunk.get().getObject()->getStartingPosition()),
				std::forward_as_tuple(addedChunk.get(), std::move(VAOFromPool))).first->second.vertexArrayToRegenerate.getObject();

			generateChunkMesh(VAO, *addedChunk.get().getObject());
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
		if (regen->second.chunkFromPool.isInUsssse())
		{
			//If Chunk has no neighbours - then it can be regenerated
			const glm::ivec3& chunkStartingPosition = regen->second.chunkFromPool.getObject()->getStartingPosition();
			if (m_chunks.find(getNeighbouringChunkPosition(chunkStartingPosition, eDirection::Left)) != m_chunks.cend() &&
				m_chunks.find(getNeighbouringChunkPosition(chunkStartingPosition, eDirection::Right)) != m_chunks.cend() &&
				m_chunks.find(getNeighbouringChunkPosition(chunkStartingPosition, eDirection::Back)) != m_chunks.cend() &&
				m_chunks.find(getNeighbouringChunkPosition(chunkStartingPosition, eDirection::Forward)) != m_chunks.cend())
			{

				generateChunkMesh(*regen->second.vertexArrayToRegenerate.getObject(), *regen->second.chunkFromPool.getObject());
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
			const glm::ivec3& chunkStartingPosition = iter->second.chunkFromPool.getObject()->getStartingPosition();
			
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
			neighbouringChunk = chunk->second.getObject();
		}
		break;
	}
	case eDirection::Right :
	{
		auto chunk = m_chunks.find(glm::ivec3(chunkStartingPosition.x + Utilities::CHUNK_WIDTH, chunkStartingPosition.y, chunkStartingPosition.z));
		if (chunk != m_chunks.cend())
		{
			neighbouringChunk = chunk->second.getObject();
		}
		break;
	}
	case eDirection::Forward :
	{
		auto chunk = m_chunks.find(glm::ivec3(chunkStartingPosition.x, chunkStartingPosition.y, chunkStartingPosition.z + Utilities::CHUNK_DEPTH));
		if (chunk != m_chunks.cend())
		{
			neighbouringChunk = chunk->second.getObject();
		}
		break;
	}
	case eDirection::Back :
	{
		auto chunk = m_chunks.find(glm::ivec3(chunkStartingPosition.x, chunkStartingPosition.y, chunkStartingPosition.z - Utilities::CHUNK_DEPTH));
		if (chunk != m_chunks.cend())
		{
			neighbouringChunk = chunk->second.getObject();
		}
		break;
	}
	default: 
		assert(false);
	}

	return neighbouringChunk;
}

