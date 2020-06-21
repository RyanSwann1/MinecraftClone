#include "MeshGenerator.h"
#include "ChunkManager.h"
#include "NeighbouringChunks.h"

namespace
{
	constexpr std::array<glm::vec2, 4> TEXT_COORDS =
	{
		glm::vec2(0.0f, 0.0f),
		glm::vec2(1.0f, 0.0f),
		glm::vec2(1.0f, 1.0f),
		glm::vec2(0.0f, 1.0f)
	};

	//Cube Position Coords
	constexpr std::array<glm::vec3, 4> CUBE_FACE_FRONT = { glm::vec3(0, 0, 1), glm::vec3(1, 0, 1), glm::vec3(1, 1, 1), glm::vec3(0, 1, 1) };
	constexpr std::array<glm::vec3, 4> CUBE_FACE_BACK = { glm::vec3(1, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0), glm::vec3(1, 1, 0) };

	constexpr std::array<glm::vec3, 4> CUBE_FACE_LEFT = { glm::vec3(0, 0, 0), glm::vec3(0, 0, 1), glm::vec3(0, 1, 1), glm::vec3(0, 1, 0) };
	constexpr std::array<glm::vec3, 4> CUBE_FACE_RIGHT = { glm::vec3(1, 0, 1), glm::vec3(1, 0, 0), glm::vec3(1, 1, 0), glm::vec3(1, 1, 1) };

	constexpr std::array<glm::vec3, 4> CUBE_FACE_TOP = { glm::vec3(0, 1, 1), glm::vec3(1, 1, 1), glm::vec3(1, 1, 0), glm::vec3(0, 1, 0) };
	constexpr std::array<glm::vec3, 4> CUBE_FACE_BOTTOM = { glm::vec3(0, 0, 0), glm::vec3(1, 0, 0), glm::vec3(1, 0, 1), glm::vec3(0, 0, 1) };

	constexpr std::array<glm::vec3, 4> FIRST_DIAGONAL_FACE = { glm::vec3(0, 0, 0), glm::vec3(1, 0, 1), glm::vec3(1, 1, 1), glm::vec3(0, 1, 0) };
	constexpr std::array<glm::vec3, 4> SECOND_DIAGONAL_FACE = { glm::vec3(0, 0, 1), glm::vec3(1, 0, 0), glm::vec3(1, 1, 0), glm::vec3(0, 1, 1) };

	//PickUp Position Coords
	constexpr std::array<glm::vec3, 4> PICKUP_CUBE_FACE_FRONT = { glm::vec3(0, 0, 0.25f), glm::vec3(0.25f, 0, 0.25f), glm::vec3(0.25f, 0.25f, 0.25f), glm::vec3(0, 0.25f, 0.25f) };
	constexpr std::array<glm::vec3, 4> PICKUP_CUBE_FACE_BACK = { glm::vec3(0.25f, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0.25f, 0), glm::vec3(0.25f, 0.25f, 0) };

	constexpr std::array<glm::vec3, 4> PICKUP_CUBE_FACE_LEFT = { glm::vec3(0, 0, 0), glm::vec3(0, 0, 0.25f), glm::vec3(0, 0.25f, 0.25f), glm::vec3(0, 0.25f, 0) };
	constexpr std::array<glm::vec3, 4> PICKUP_CUBE_FACE_RIGHT = { glm::vec3(0.25f, 0, 0.25f), glm::vec3(0.25f, 0, 0), glm::vec3(0.25f, 0.25f, 0), glm::vec3(0.25f, 0.25f, 0.25f) };

	constexpr std::array<glm::vec3, 4> PICKUP_CUBE_FACE_TOP = { glm::vec3(0, 0.25f, 0.25f), glm::vec3(0.25f, 0.25f, 0.25f), glm::vec3(0.25f, 0.25f, 0), glm::vec3(0, 0.25f, 0) };
	constexpr std::array<glm::vec3, 4> PICKUP_CUBE_FACE_BOTTOM = { glm::vec3(0, 0, 0), glm::vec3(0.25f, 0, 0), glm::vec3(0.25f, 0, 0.25f), glm::vec3(0, 0, 0.25f) };

	constexpr std::array<glm::vec3, 4> PICKUP_FIRST_DIAGONAL_FACE = { glm::vec3(0, 0, 0), glm::vec3(0.25f, 0, 0.25f), glm::vec3(0.25f, 0.25f, 0.25f), glm::vec3(0, 0.25f, 0) };
	constexpr std::array<glm::vec3, 4> PICKUP_SECOND_DIAGONAL_FACE = { glm::vec3(0, 0, 0.25f), glm::vec3(0.25f, 0, 0), glm::vec3(0.25f, 0.25f, 0), glm::vec3(0, 0.25f, 0.25f) };

	void getTextCoords(std::vector<glm::vec3>& textCoords, eCubeSide cubeSide, eCubeType cubeType)
	{
		eTextureLayer textureLayer;
		switch (cubeType)
		{
		case eCubeType::Dirt:
			textureLayer = eTextureLayer::Dirt;
			break;
		case eCubeType::Stone:
			textureLayer = eTextureLayer::Stone;
			break;
		case eCubeType::Sand:
			textureLayer = eTextureLayer::Sand;
			break;
		case eCubeType::Grass:
			switch (cubeSide)
			{
			case eCubeSide::Back:
			case eCubeSide::Front:
			case eCubeSide::Left:
			case eCubeSide::Right:
				textureLayer = eTextureLayer::GrassSide;
				break;
			case eCubeSide::Top:
				textureLayer = eTextureLayer::Grass;
				break;
			case eCubeSide::Bottom:
				textureLayer = eTextureLayer::Dirt;
				break;
			}
			break;
		case eCubeType::Log:
			textureLayer = eTextureLayer::Log;
			break;
		case eCubeType::LogTop:
			switch (cubeSide)
			{
			case eCubeSide::Back:
			case eCubeSide::Front:
			case eCubeSide::Left:
			case eCubeSide::Right:
				textureLayer = eTextureLayer::Log;
				break;
			case eCubeSide::Top:
				textureLayer = eTextureLayer::LogTop;
				break;
			}
			break;
		case eCubeType::Leaves:
			textureLayer = eTextureLayer::Leaves;
			break;
		case eCubeType::Cactus:
			textureLayer = eTextureLayer::Cactus;
			break;
		case eCubeType::CactusTop:
			switch (cubeSide)
			{
			case eCubeSide::Back:
			case eCubeSide::Front:
			case eCubeSide::Left:
			case eCubeSide::Right:
				textureLayer = eTextureLayer::Cactus;
				break;
			case eCubeSide::Top:
				textureLayer = eTextureLayer::CactusTop;
				break;
			}
			break;
		case eCubeType::Water:
			textureLayer = eTextureLayer::Water;
			break;
		case eCubeType::Shrub:
			textureLayer = eTextureLayer::Shrub;
			break;
		case eCubeType::TallGrass:
			textureLayer = eTextureLayer::TallGrass;
			break;

		default:
			textureLayer = eTextureLayer::Error;
		}

		assert(textureLayer != eTextureLayer::Error);
		for (const auto& i : TEXT_COORDS)
		{
			textCoords.emplace_back(i.x, i.y, static_cast<int>(textureLayer));
		}
	}

	void addItemCubeFace(VertexBuffer& vertexBuffer, eCubeType cubeType, eCubeSide cubeSide, const glm::vec3& cubePosition)
	{
		glm::vec3 position = cubePosition;
		switch (cubeSide)
		{
		case eCubeSide::Front:
			for (const glm::vec3& i : PICKUP_CUBE_FACE_FRONT)
			{
				position += i;
				vertexBuffer.positions.emplace_back(position.x, position.y, position.z);
				position = cubePosition;

				vertexBuffer.lightIntensityVertices.push_back(MeshGenerator::FRONT_FACE_LIGHTING_INTENSITY);
			}

			getTextCoords(vertexBuffer.textCoords, cubeSide, cubeType);

			break;
		case eCubeSide::Back:
			for (const glm::vec3& i : PICKUP_CUBE_FACE_BACK)
			{
				position += i;
				vertexBuffer.positions.emplace_back(position.x, position.y, position.z);
				position = cubePosition;

				vertexBuffer.lightIntensityVertices.push_back(MeshGenerator::BACK_FACE_LIGHTING_INTENSITY);
			}

			getTextCoords(vertexBuffer.textCoords, cubeSide, cubeType);

			break;
		case eCubeSide::Left:
			for (const glm::vec3& i : PICKUP_CUBE_FACE_LEFT)
			{
				position += i;
				vertexBuffer.positions.emplace_back(position.x, position.y, position.z);
				position = cubePosition;

				vertexBuffer.lightIntensityVertices.push_back(MeshGenerator::LEFT_FACE_LIGHTING_INTENSITY);
			}
			getTextCoords(vertexBuffer.textCoords, cubeSide, cubeType);
			break;
		case eCubeSide::Right:
			for (const glm::vec3& i : PICKUP_CUBE_FACE_RIGHT)
			{
				position += i;
				vertexBuffer.positions.emplace_back(position.x, position.y, position.z);
				position = cubePosition;

				vertexBuffer.lightIntensityVertices.push_back(MeshGenerator::RIGHT_FACE_LIGHTING_INTENSITY);
			}

			getTextCoords(vertexBuffer.textCoords, cubeSide, cubeType);

			break;
		case eCubeSide::Top:
			for (const glm::vec3& i : PICKUP_CUBE_FACE_TOP)
			{
				position += i;
				vertexBuffer.positions.emplace_back(position.x, position.y, position.z);
				position = cubePosition;

				vertexBuffer.lightIntensityVertices.push_back(MeshGenerator::TOP_LIGHTING_INTENSITY);
			}
			getTextCoords(vertexBuffer.textCoords, cubeSide, cubeType);

			break;
		case eCubeSide::Bottom:
			for (const glm::vec3& i : PICKUP_CUBE_FACE_BOTTOM)
			{
				position += i;
				vertexBuffer.positions.emplace_back(position.x, position.y, position.z);
				position = cubePosition;

				vertexBuffer.lightIntensityVertices.push_back(MeshGenerator::BOTTOM_FACE_LIGHTING_INTENSITY);
			}

			getTextCoords(vertexBuffer.textCoords, cubeSide, cubeType);

			break;
		}

		for (unsigned int i : MeshGenerator::CUBE_FACE_INDICIES)
		{
			vertexBuffer.indicies.emplace_back(i + vertexBuffer.elementBufferIndex);
		}

		vertexBuffer.elementBufferIndex += MeshGenerator::CUBE_FACE_INDICIE_COUNT;
	}
}

void generateOuterChunkMesh(VertexArray& chunkMesh, const Chunk& chunk, const NeighbouringChunks& neighbouringChunks);
void generateInnerChunkMesh(VertexArray& chunkMesh, const Chunk& chunk);
void generateChunkInnerCubeMesh(const glm::ivec3& position, const Chunk& chunk, eCubeType cubeType, VertexArray& chunkMesh);
void generateChunkOuterCubeMesh(const glm::ivec3& position, const Chunk& chunk, const NeighbouringChunks& neighbouringChunks,
	eCubeType cubeType, VertexArray& chunkMesh);
void addCubeFace(VertexBuffer& vertexBuffer, eCubeType cubeType, eCubeSide cubeSide, const glm::ivec3& cubePosition,
	bool transparent, bool shadow = false);
void addDiagonalCubeFace(VertexBuffer& vertexBuffer, eCubeType cubeType, const glm::ivec3& cubePosition,
	const std::array<glm::vec3, 4>& diagonalFace, bool shadow = false);

bool isFacingTransparentCube(const glm::ivec3& cubePosition, const Chunk& chunk);
bool isFacingOpaqueCube(const glm::ivec3& cubePosition, const Chunk& chunk);



void MeshGenerator::generateChunkMesh(VertexArray& chunkMesh, const Chunk& chunk, const NeighbouringChunks& neighbouringChunks)
{
	generateInnerChunkMesh(chunkMesh, chunk);
	generateOuterChunkMesh(chunkMesh, chunk, neighbouringChunks);

	if (!chunkMesh.m_opaqueVertexBuffer.indicies.empty())
	{
		chunkMesh.m_opaqueVertexBuffer.bindToVAO = true;
	}

	if (!chunkMesh.m_transparentVertexBuffer.indicies.empty())
	{
		chunkMesh.m_transparentVertexBuffer.bindToVAO = true;
	}
}

void MeshGenerator::generatePickUpMesh(VertexBuffer& pickUpMesh, eCubeType cubeType, const glm::vec3& position)
{
	addItemCubeFace(pickUpMesh, cubeType, eCubeSide::Left, position);
	addItemCubeFace(pickUpMesh, cubeType, eCubeSide::Right, position);
	addItemCubeFace(pickUpMesh, cubeType, eCubeSide::Top, position);
	addItemCubeFace(pickUpMesh, cubeType, eCubeSide::Bottom, position);
	addItemCubeFace(pickUpMesh, cubeType, eCubeSide::Front, position);
	addItemCubeFace(pickUpMesh, cubeType, eCubeSide::Back, position);

	pickUpMesh.bindToVAO = true;
}

void generateOuterChunkMesh(VertexArray& chunkMesh, const Chunk& chunk, const NeighbouringChunks& neighbouringChunks)
{
	const glm::ivec3& chunkStartingPosition = chunk.getStartingPosition();
	const glm::ivec3& chunkEndingPosition = chunk.getEndingPosition();

	//Vertical
	for (int z = chunkStartingPosition.z; z < chunkEndingPosition.z; ++z)
	{
		for (int y = chunkStartingPosition.y; y < chunkEndingPosition.y; ++y)
		{
			eCubeType cubeType = static_cast<eCubeType>(chunk.getCubeDetailsWithoutBoundsCheck({ chunkStartingPosition.x, y, z }));
			if (cubeType != eCubeType::Air)
			{
				generateChunkOuterCubeMesh({ chunkStartingPosition.x, y, z }, chunk, neighbouringChunks, cubeType, chunkMesh);
			}
		}
	}

	for (int z = chunkStartingPosition.z; z < chunkEndingPosition.z; ++z)
	{
		for (int y = chunkStartingPosition.y; y < chunkEndingPosition.y; ++y)
		{
			eCubeType cubeType = static_cast<eCubeType>(chunk.getCubeDetailsWithoutBoundsCheck({ chunkEndingPosition.x - 1, y, z }));

			if (cubeType != eCubeType::Air)
			{
				generateChunkOuterCubeMesh({ chunkEndingPosition.x - 1, y, z }, chunk, neighbouringChunks, cubeType, chunkMesh);
			}
		}
	}

	//Horizontal
	for (int y = chunkStartingPosition.y; y < chunkEndingPosition.y; ++y)
	{
		for (int x = chunkStartingPosition.x; x < chunkEndingPosition.x; ++x)
		{
			eCubeType cubeType = static_cast<eCubeType>(chunk.getCubeDetailsWithoutBoundsCheck({ x, y, chunkStartingPosition.z }));

			if (cubeType != eCubeType::Air)
			{
				generateChunkOuterCubeMesh({ x, y, chunkStartingPosition.z }, chunk, neighbouringChunks, cubeType, chunkMesh);
			}
		}
	}

	for (int y = chunkStartingPosition.y; y < chunkEndingPosition.y; ++y)
	{
		for (int x = chunkStartingPosition.x; x < chunkEndingPosition.x; ++x)
		{
			eCubeType cubeType = static_cast<eCubeType>(chunk.getCubeDetailsWithoutBoundsCheck({ x, y, chunkEndingPosition.z - 1 }));

			if (cubeType != eCubeType::Air)
			{
				generateChunkOuterCubeMesh({ x, y, chunkEndingPosition.z - 1 }, chunk, neighbouringChunks, cubeType, chunkMesh);
			}
		}
	}
}

void generateInnerChunkMesh(VertexArray& chunkMesh, const Chunk& chunk)
{
	const glm::ivec3& chunkStartingPosition = chunk.getStartingPosition();
	const glm::ivec3& chunkEndingPosition = chunk.getEndingPosition();

	for (int z = chunkStartingPosition.z + 1; z < chunkEndingPosition.z - 1; ++z)
	{
		for (int y = chunkStartingPosition.y + 1; y < chunkEndingPosition.y - 1; ++y)
		{
			for (int x = chunkStartingPosition.x + 1; x < chunkEndingPosition.x - 1; ++x)
			{
				glm::ivec3 position(x, y, z);
				eCubeType cubeType = static_cast<eCubeType>(chunk.getCubeDetailsWithoutBoundsCheck(position));
				if (cubeType != eCubeType::Air)
				{
					generateChunkInnerCubeMesh(position, chunk, cubeType, chunkMesh);
				}
			}
		}
	}
}

void generateChunkInnerCubeMesh(const glm::ivec3& position, const Chunk& chunk, eCubeType cubeType, VertexArray& chunkMesh)
{
	assert(chunk.isPositionInBounds(position));

	switch (cubeType)
	{
	case eCubeType::Water:

		if(isFacingOpaqueCube({ position.x, position.y + 1, position.z }, chunk))
		{
			addCubeFace(chunkMesh.m_transparentVertexBuffer, cubeType, eCubeSide::Top, position, true);
		}

		break;
	case eCubeType::Leaves:
		if (isFacingOpaqueCube({ position.x - 1, position.y, position.z }, chunk))
		{
			addCubeFace(chunkMesh.m_transparentVertexBuffer, cubeType, eCubeSide::Left, position, true);
		}

		if (isFacingOpaqueCube({ position.x + 1, position.y, position.z }, chunk))
		{
			addCubeFace(chunkMesh.m_transparentVertexBuffer, cubeType, eCubeSide::Right, position, true);
		}

		if (isFacingOpaqueCube({ position.x, position.y, position.z + 1 }, chunk))
		{
			addCubeFace(chunkMesh.m_transparentVertexBuffer, cubeType, eCubeSide::Front, position, true);
		}

		if (isFacingOpaqueCube({ position.x, position.y, position.z - 1 }, chunk))
		{
			addCubeFace(chunkMesh.m_transparentVertexBuffer, cubeType, eCubeSide::Back, position, true);
		}

		//Top Face
		if (position.y == Globals::CHUNK_HEIGHT - 1 ||
			isFacingOpaqueCube({ position.x, position.y + 1, position.z }, chunk))
		{
			addCubeFace(chunkMesh.m_transparentVertexBuffer, cubeType, eCubeSide::Top, position, true);
		}

		//Bottom Face
		if (isFacingOpaqueCube({ position.x, position.y - 1, position.z }, chunk))
		{
			addCubeFace(chunkMesh.m_transparentVertexBuffer, cubeType, eCubeSide::Bottom, position, true);
		}
		break;
	case eCubeType::TallGrass:
	case eCubeType::Shrub:
	{
		bool shadow = chunk.isCubeBelowCovering(position);

		addDiagonalCubeFace(chunkMesh.m_transparentVertexBuffer, cubeType, position, FIRST_DIAGONAL_FACE, shadow);
		addDiagonalCubeFace(chunkMesh.m_transparentVertexBuffer, cubeType, position, SECOND_DIAGONAL_FACE, shadow);
	}

		break;
	default:
	{
		bool shadow = chunk.isCubeBelowCovering(position);

		if (isFacingTransparentCube({ position.x - 1, position.y, position.z }, chunk))
		{
			addCubeFace(chunkMesh.m_opaqueVertexBuffer, cubeType, eCubeSide::Left, position, false, shadow);
		}

		if (isFacingTransparentCube({ position.x + 1, position.y, position.z }, chunk))
		{
			addCubeFace(chunkMesh.m_opaqueVertexBuffer, cubeType, eCubeSide::Right, position, false, shadow);
		}

		if (isFacingTransparentCube({ position.x, position.y, position.z + 1 }, chunk))
		{
			addCubeFace(chunkMesh.m_opaqueVertexBuffer, cubeType, eCubeSide::Front, position, false, shadow);
		}

		if (isFacingTransparentCube({ position.x, position.y, position.z - 1 }, chunk))
		{
			addCubeFace(chunkMesh.m_opaqueVertexBuffer, cubeType, eCubeSide::Back, position, false, shadow);
		}

		if (isFacingTransparentCube({ position.x, position.y - 1, position.z}, chunk))
		{
			addCubeFace(chunkMesh.m_opaqueVertexBuffer, cubeType, eCubeSide::Bottom, position, false, shadow);
		}

		if (cubeType == eCubeType::LogTop)
		{
			addCubeFace(chunkMesh.m_opaqueVertexBuffer, cubeType, eCubeSide::Top, position, false, shadow);
		}
		else
		{
			if (position.y == Globals::CHUNK_HEIGHT - 1 ||
				isFacingTransparentCube({ position.x, position.y + 1, position.z }, chunk))
			{
				addCubeFace(chunkMesh.m_opaqueVertexBuffer, cubeType, eCubeSide::Top, position, false, shadow);
			}
		}
	}
	}
}

void generateChunkOuterCubeMesh(const glm::ivec3& position, const Chunk& chunk, const NeighbouringChunks& neighbouringChunks, eCubeType cubeType, VertexArray& chunkMesh)
{
	assert(chunk.isPositionInBounds(position));

	switch (cubeType)
	{
	case eCubeType::Water:
		if (isFacingOpaqueCube({ position.x, position.y + 1, position.z }, chunk))
		{
			addCubeFace(chunkMesh.m_transparentVertexBuffer, cubeType, eCubeSide::Top, position, true);
		}
		break;
	case eCubeType::Leaves:
	{
		//Left Face
		glm::ivec3 leftPosition(position.x - 1, position.y, position.z);
		if (chunk.isPositionInBounds(leftPosition))
		{
			if (isFacingOpaqueCube(leftPosition, chunk))
			{
				addCubeFace(chunkMesh.m_transparentVertexBuffer, cubeType, eCubeSide::Left, position, true);
			}
		}
		else if (isFacingOpaqueCube(leftPosition, neighbouringChunks.chunks[static_cast<int>(eDirection::Left)]))
		{
			addCubeFace(chunkMesh.m_transparentVertexBuffer, cubeType, eCubeSide::Left, position, true);
		}

		//Right Face
		glm::ivec3 rightPosition(position.x + 1, position.y, position.z);
		if (chunk.isPositionInBounds(rightPosition))
		{
			if (isFacingOpaqueCube(rightPosition, chunk))
			{
				addCubeFace(chunkMesh.m_transparentVertexBuffer, cubeType, eCubeSide::Right, position, true);
			}
		}
		else if (isFacingOpaqueCube(rightPosition, neighbouringChunks.chunks[static_cast<int>(eDirection::Right)]))
		{
			addCubeFace(chunkMesh.m_transparentVertexBuffer, cubeType, eCubeSide::Right, position, true);
		}

		//Forward Face
		glm::ivec3 forwardPosition(position.x, position.y, position.z + 1);
		if (chunk.isPositionInBounds(forwardPosition))
		{
			if (isFacingOpaqueCube(forwardPosition, chunk))
			{
				addCubeFace(chunkMesh.m_transparentVertexBuffer, cubeType, eCubeSide::Front, position, true);
			}
		}
		else if (isFacingOpaqueCube(forwardPosition, neighbouringChunks.chunks[static_cast<int>(eDirection::Forward)]))
		{
			addCubeFace(chunkMesh.m_transparentVertexBuffer, cubeType, eCubeSide::Front, position, true);
		}

		//Back Face
		glm::ivec3 backPosition(position.x, position.y, position.z - 1);
		if (chunk.isPositionInBounds(backPosition))
		{
			if (isFacingOpaqueCube(backPosition, chunk))
			{
				addCubeFace(chunkMesh.m_transparentVertexBuffer, cubeType, eCubeSide::Back, position, true);
			}
		}
		else if (isFacingOpaqueCube(backPosition, neighbouringChunks.chunks[static_cast<int>(eDirection::Back)]))
		{
			addCubeFace(chunkMesh.m_transparentVertexBuffer, cubeType, eCubeSide::Back, position, true);
		}


		//Top Face
		if (position.y == Globals::CHUNK_HEIGHT - 1 ||
			isFacingOpaqueCube({ position.x, position.y + 1, position.z }, chunk))
		{
			addCubeFace(chunkMesh.m_transparentVertexBuffer, cubeType, eCubeSide::Top, position, true);
		}

		//Bottom Face
		if (position.y > 0 && isFacingOpaqueCube({ position.x, position.y - 1, position.z }, chunk))
		{
			addCubeFace(chunkMesh.m_transparentVertexBuffer, cubeType, eCubeSide::Bottom, position, true);
		}
	}
		break;
	case eCubeType::TallGrass:
	case eCubeType::Shrub:
	{
		bool shadow = chunk.isCubeBelowCovering(position);

		addDiagonalCubeFace(chunkMesh.m_transparentVertexBuffer, cubeType, position, FIRST_DIAGONAL_FACE, shadow);
		addDiagonalCubeFace(chunkMesh.m_transparentVertexBuffer, cubeType, position, SECOND_DIAGONAL_FACE, shadow);
	}

		break;
	default:
	{
		bool shadow = chunk.isCubeBelowCovering(position);

		//Left Face
		glm::ivec3 leftPosition(position.x - 1, position.y, position.z);
		if (chunk.isPositionInBounds(leftPosition))
		{
			if (isFacingTransparentCube(leftPosition, chunk))
			{
				addCubeFace(chunkMesh.m_opaqueVertexBuffer, cubeType, eCubeSide::Left, position, false, shadow);
			}
		}
		else if (isFacingTransparentCube(leftPosition, neighbouringChunks.chunks[static_cast<int>(eDirection::Left)]))
		{
			addCubeFace(chunkMesh.m_opaqueVertexBuffer, cubeType, eCubeSide::Left, position, false, shadow);
		}

		//Right Face
		glm::ivec3 rightPosition(position.x + 1, position.y, position.z);
		if (chunk.isPositionInBounds(rightPosition))
		{
			if (isFacingTransparentCube(rightPosition, chunk))
			{
				addCubeFace(chunkMesh.m_opaqueVertexBuffer, cubeType, eCubeSide::Right, position, false, shadow);
			}
		}
		else if (isFacingTransparentCube(rightPosition, neighbouringChunks.chunks[static_cast<int>(eDirection::Right)]))
		{
			addCubeFace(chunkMesh.m_opaqueVertexBuffer, cubeType, eCubeSide::Right, position, false, shadow);
		}

		//Forward Face
		glm::ivec3 forwardPosition(position.x, position.y, position.z + 1);
		if (chunk.isPositionInBounds(forwardPosition))
		{
			if (isFacingTransparentCube(forwardPosition, chunk))
			{
				addCubeFace(chunkMesh.m_opaqueVertexBuffer, cubeType, eCubeSide::Front, position, false, shadow);
			}
		}
		else if (isFacingTransparentCube(forwardPosition, neighbouringChunks.chunks[static_cast<int>(eDirection::Forward)]))
		{
			addCubeFace(chunkMesh.m_opaqueVertexBuffer, cubeType, eCubeSide::Front, position, false, shadow);
		}

		//Back Face
		glm::ivec3 backPosition(position.x, position.y, position.z - 1);
		if (chunk.isPositionInBounds(backPosition))
		{
			if (isFacingTransparentCube(backPosition, chunk))
			{
				addCubeFace(chunkMesh.m_opaqueVertexBuffer, cubeType, eCubeSide::Back, position, false, shadow);
			}
		}
		else if (isFacingTransparentCube(backPosition, neighbouringChunks.chunks[static_cast<int>(eDirection::Back)]))
		{
			addCubeFace(chunkMesh.m_opaqueVertexBuffer, cubeType, eCubeSide::Back, position, false, shadow);
		}

		//Bottom Face
		if (position.y > 0 && isFacingTransparentCube({ position.x, position.y - 1, position.z }, chunk))
		{
			addCubeFace(chunkMesh.m_opaqueVertexBuffer, cubeType, eCubeSide::Bottom, position, false, shadow);
		}

		if (cubeType == eCubeType::LogTop)
		{
			addCubeFace(chunkMesh.m_opaqueVertexBuffer, cubeType, eCubeSide::Top, position, false, shadow);
		}
		else
		{
			//Top Face
			if (position.y == Globals::CHUNK_HEIGHT - 1 ||
				isFacingTransparentCube({ position.x, position.y + 1, position.z }, chunk))
			{
				addCubeFace(chunkMesh.m_opaqueVertexBuffer, cubeType, eCubeSide::Top, position, false, shadow);
			}
		}
	}
	}
}

void addCubeFace(VertexBuffer& vertexBuffer, eCubeType cubeType, eCubeSide cubeSide, const glm::ivec3& cubePosition, bool transparent, bool shadow)
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

			if (shadow)
			{
				vertexBuffer.lightIntensityVertices.push_back(MeshGenerator::SHADOW_INTENSITY);
			}
			else if (transparent)
			{
				vertexBuffer.lightIntensityVertices.push_back(MeshGenerator::DEFAULT_LIGHTING_INTENSITY);
			}
			else
			{
				vertexBuffer.lightIntensityVertices.push_back(MeshGenerator::FRONT_FACE_LIGHTING_INTENSITY);
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

			if (shadow)
			{
				vertexBuffer.lightIntensityVertices.push_back(MeshGenerator::SHADOW_INTENSITY);
			}
			else if (transparent)
			{
				vertexBuffer.lightIntensityVertices.push_back(MeshGenerator::DEFAULT_LIGHTING_INTENSITY);
			}
			else
			{
				vertexBuffer.lightIntensityVertices.push_back(MeshGenerator::BACK_FACE_LIGHTING_INTENSITY);
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

			if (shadow)
			{
				vertexBuffer.lightIntensityVertices.push_back(MeshGenerator::SHADOW_INTENSITY);
			}
			else if (transparent)
			{
				vertexBuffer.lightIntensityVertices.push_back(MeshGenerator::DEFAULT_LIGHTING_INTENSITY);
			}
			else
			{
				vertexBuffer.lightIntensityVertices.push_back(MeshGenerator::LEFT_FACE_LIGHTING_INTENSITY);
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

			if (shadow)
			{
				vertexBuffer.lightIntensityVertices.push_back(MeshGenerator::SHADOW_INTENSITY);
			}
			else if (transparent)
			{
				vertexBuffer.lightIntensityVertices.push_back(MeshGenerator::DEFAULT_LIGHTING_INTENSITY);
			}
			else
			{
				vertexBuffer.lightIntensityVertices.push_back(MeshGenerator::RIGHT_FACE_LIGHTING_INTENSITY);
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

			if (shadow)
			{
				vertexBuffer.lightIntensityVertices.push_back(MeshGenerator::SHADOW_INTENSITY);
			}
			else if (transparent)
			{
				vertexBuffer.lightIntensityVertices.push_back(MeshGenerator::DEFAULT_LIGHTING_INTENSITY);
			}
			else
			{
				vertexBuffer.lightIntensityVertices.push_back(MeshGenerator::TOP_LIGHTING_INTENSITY);
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

			if (shadow)
			{
				vertexBuffer.lightIntensityVertices.push_back(MeshGenerator::SHADOW_INTENSITY);
			}
			else if (transparent)
			{
				vertexBuffer.lightIntensityVertices.push_back(MeshGenerator::DEFAULT_LIGHTING_INTENSITY);
			}
			else
			{
				vertexBuffer.lightIntensityVertices.push_back(MeshGenerator::BOTTOM_FACE_LIGHTING_INTENSITY);
			}
		}

		getTextCoords(vertexBuffer.textCoords, cubeSide, cubeType);

		break;
	}

	for (unsigned int i : MeshGenerator::CUBE_FACE_INDICIES)
	{
		vertexBuffer.indicies.emplace_back(i + vertexBuffer.elementBufferIndex);
	}

	vertexBuffer.elementBufferIndex += MeshGenerator::CUBE_FACE_INDICIE_COUNT;
}

void addDiagonalCubeFace(VertexBuffer& vertexBuffer, eCubeType cubeType, const glm::ivec3& cubePosition, const std::array<glm::vec3, 4>& diagonalFace, bool shadow)
{
	//Positions
	glm::ivec3 position = cubePosition;
	for (const glm::vec3& i : diagonalFace)
	{
		position += i;
		vertexBuffer.positions.emplace_back(position.x, position.y, position.z);
		position = cubePosition;

		//Lighting
		if (shadow)
		{
			vertexBuffer.lightIntensityVertices.push_back(MeshGenerator::SHADOW_INTENSITY);
		}
		else
		{
			vertexBuffer.lightIntensityVertices.push_back(MeshGenerator::DEFAULT_LIGHTING_INTENSITY);
		}

	}

	//Texture Coordinates
	getTextCoords(vertexBuffer.textCoords, eCubeSide::Front, cubeType);

	//Indicies
	for (unsigned int i : MeshGenerator::CUBE_FACE_INDICIES)
	{
		vertexBuffer.indicies.emplace_back(i + vertexBuffer.elementBufferIndex);
	}

	vertexBuffer.elementBufferIndex += MeshGenerator::CUBE_FACE_INDICIE_COUNT;
}

bool isFacingTransparentCube(const glm::ivec3& cubePosition, const Chunk& chunk)
{
	eCubeType cubeType = static_cast<eCubeType>(chunk.getCubeDetailsWithoutBoundsCheck(cubePosition));
	if (cubeType != eCubeType::Air)
	{
		return Globals::TRANSPARENT_CUBE_TYPES.isMatch(cubeType);
	}
	else
	{
		return true;
	}
}

bool isFacingOpaqueCube(const glm::ivec3& cubePosition, const Chunk& chunk)
{
	eCubeType cubeType = static_cast<eCubeType>(chunk.getCubeDetailsWithoutBoundsCheck(cubePosition));
	if (cubeType != eCubeType::Air)
	{
		return Globals::OPAQUE_CUBE_TYPES.isMatch(cubeType);
	}
	else
	{
		return true;
	}
}