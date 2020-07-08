#include "MeshGenerator.h"
#include "ChunkManager.h"
#include "NeighbouringChunks.h"

namespace
{
	constexpr int CUBE_FACE_INDICIE_COUNT = 4;

	constexpr std::array<unsigned int, 6> CUBE_FACE_INDICIES =
	{
		0, 1, 2,
		2, 3, 0
	};

	constexpr float DEFAULT_LIGHTING_INTENSITY = 1.0f;
	constexpr float TOP_LIGHTING_INTENSITY = 1.0f;
	constexpr float FRONT_FACE_LIGHTING_INTENSITY = 0.8f;
	constexpr float BACK_FACE_LIGHTING_INTENSITY = 0.8f;
	constexpr float LEFT_FACE_LIGHTING_INTENSITY = 0.75f;
	constexpr float RIGHT_FACE_LIGHTING_INTENSITY = 0.75f;
	constexpr float SHADOW_INTENSITY = 0.4f;
	constexpr float BOTTOM_FACE_LIGHTING_INTENSITY = 0.4f;
	constexpr float WATER_OFFSET_Y = 0.2f;

	constexpr std::array<glm::vec2, 4> TEXT_COORDS =
	{
		glm::vec2(0.0f, 0.0f),
		glm::vec2(1.0f, 0.0f),
		glm::vec2(1.0f, 1.0f),
		glm::vec2(0.0f, 1.0f)
	};

	std::array<glm::vec3, 4> getDestroyBlockTextCoords(const glm::vec2& startingPosition, const glm::vec2& endingPosition)
	{
		//assert(endingPosition.x > startingPosition.x);
		float x = endingPosition.x - startingPosition.x;
		assert(endingPosition.y > startingPosition.y);
		float y = endingPosition.y - startingPosition.y;

		return
		{
			glm::vec3(startingPosition.x, startingPosition.y, 0.0f),
			glm::vec3(startingPosition.x + x, startingPosition.y, 0.0f),
			glm::vec3(startingPosition.x + x, startingPosition.y + y, 0.0f),
			glm::vec3(startingPosition.x, startingPosition.y + y, 0.0f)
		};
	};

	//Cube Position Coords
	constexpr std::array<glm::vec3, 4> CUBE_FACE_FRONT = 
	{ 
		glm::vec3(0, 0, Globals::CUBE_FACE_SIZE), 
		glm::vec3(Globals::CUBE_FACE_SIZE, 0, Globals::CUBE_FACE_SIZE), 
		glm::vec3(Globals::CUBE_FACE_SIZE, Globals::CUBE_FACE_SIZE, Globals::CUBE_FACE_SIZE), 
		glm::vec3(0, Globals::CUBE_FACE_SIZE, Globals::CUBE_FACE_SIZE) 
	};
	constexpr std::array<glm::vec3, 4> CUBE_FACE_BACK = 
	{ 
		glm::vec3(Globals::CUBE_FACE_SIZE, 0, 0), 
		glm::vec3(0, 0, 0), 
		glm::vec3(0, Globals::CUBE_FACE_SIZE, 0), 
		glm::vec3(Globals::CUBE_FACE_SIZE, Globals::CUBE_FACE_SIZE, 0) 
	};

	constexpr std::array<glm::vec3, 4> CUBE_FACE_LEFT = 
	{ 
		glm::vec3(0, 0, 0), 
		glm::vec3(0, 0, Globals::CUBE_FACE_SIZE), 
		glm::vec3(0, Globals::CUBE_FACE_SIZE, Globals::CUBE_FACE_SIZE), 
		glm::vec3(0, Globals::CUBE_FACE_SIZE, 0) 
	};
	constexpr std::array<glm::vec3, 4> CUBE_FACE_RIGHT = 
	{ 
		glm::vec3(Globals::CUBE_FACE_SIZE, 0, Globals::CUBE_FACE_SIZE), 
		glm::vec3(Globals::CUBE_FACE_SIZE, 0, 0), 
		glm::vec3(Globals::CUBE_FACE_SIZE, Globals::CUBE_FACE_SIZE, 0), 
		glm::vec3(Globals::CUBE_FACE_SIZE, Globals::CUBE_FACE_SIZE, Globals::CUBE_FACE_SIZE) 
	};

	constexpr std::array<glm::vec3, 4> CUBE_FACE_TOP = 
	{ 
		glm::vec3(0, Globals::CUBE_FACE_SIZE, Globals::CUBE_FACE_SIZE), 
		glm::vec3(Globals::CUBE_FACE_SIZE, Globals::CUBE_FACE_SIZE, Globals::CUBE_FACE_SIZE), 
		glm::vec3(Globals::CUBE_FACE_SIZE, Globals::CUBE_FACE_SIZE, 0), 
		glm::vec3(0, Globals::CUBE_FACE_SIZE, 0) 
	};
	constexpr std::array<glm::vec3, 4> CUBE_FACE_BOTTOM = 
	{ 
		glm::vec3(0, 0, 0), 
		glm::vec3(Globals::CUBE_FACE_SIZE, 0, 0), 
		glm::vec3(Globals::CUBE_FACE_SIZE, 0, Globals::CUBE_FACE_SIZE), 
		glm::vec3(0, 0, Globals::CUBE_FACE_SIZE) 
	};

	constexpr std::array<glm::vec3, 4> FIRST_DIAGONAL_FACE = 
	{ 
		glm::vec3(0, 0, 0), 
		glm::vec3(Globals::CUBE_FACE_SIZE, 0, Globals::CUBE_FACE_SIZE), 
		glm::vec3(Globals::CUBE_FACE_SIZE, Globals::CUBE_FACE_SIZE, Globals::CUBE_FACE_SIZE), 
		glm::vec3(0, Globals::CUBE_FACE_SIZE, 0) 
	};
	constexpr std::array<glm::vec3, 4> SECOND_DIAGONAL_FACE = 
	{ 
		glm::vec3(0, 0, Globals::CUBE_FACE_SIZE), 
		glm::vec3(Globals::CUBE_FACE_SIZE, 0, 0), 
		glm::vec3(Globals::CUBE_FACE_SIZE, Globals::CUBE_FACE_SIZE, 0), 
		glm::vec3(0, Globals::CUBE_FACE_SIZE, Globals::CUBE_FACE_SIZE) 
	};

	//PickUp Position Coords
	constexpr std::array<glm::vec3, 4> PICKUP_CUBE_FACE_FRONT = 
	{ 
		glm::vec3(0, 0, Globals::PICKUP_CUBE_FACE_SIZE), 
		glm::vec3(Globals::PICKUP_CUBE_FACE_SIZE, 0, Globals::PICKUP_CUBE_FACE_SIZE), 
		glm::vec3(Globals::PICKUP_CUBE_FACE_SIZE, Globals::PICKUP_CUBE_FACE_SIZE, Globals::PICKUP_CUBE_FACE_SIZE), 
		glm::vec3(0, Globals::PICKUP_CUBE_FACE_SIZE, Globals::PICKUP_CUBE_FACE_SIZE) 
	};
	constexpr std::array<glm::vec3, 4> PICKUP_CUBE_FACE_BACK = 
	{ 
		glm::vec3(Globals::PICKUP_CUBE_FACE_SIZE, 0, 0), 
		glm::vec3(0, 0, 0), 
		glm::vec3(0, Globals::PICKUP_CUBE_FACE_SIZE, 0), 
		glm::vec3(Globals::PICKUP_CUBE_FACE_SIZE, Globals::PICKUP_CUBE_FACE_SIZE, 0) 
	};

	constexpr std::array<glm::vec3, 4> PICKUP_CUBE_FACE_LEFT = 
	{ glm::vec3(0, 0, 0), 
		glm::vec3(0, 0, Globals::PICKUP_CUBE_FACE_SIZE), 
		glm::vec3(0, Globals::PICKUP_CUBE_FACE_SIZE, Globals::PICKUP_CUBE_FACE_SIZE), 
		glm::vec3(0, Globals::PICKUP_CUBE_FACE_SIZE, 0) 
	};
	constexpr std::array<glm::vec3, 4> PICKUP_CUBE_FACE_RIGHT = 
	{ 
		glm::vec3(Globals::PICKUP_CUBE_FACE_SIZE, 0, Globals::PICKUP_CUBE_FACE_SIZE), 
		glm::vec3(Globals::PICKUP_CUBE_FACE_SIZE, 0, 0), 
		glm::vec3(Globals::PICKUP_CUBE_FACE_SIZE, Globals::PICKUP_CUBE_FACE_SIZE, 0), 
		glm::vec3(Globals::PICKUP_CUBE_FACE_SIZE, Globals::PICKUP_CUBE_FACE_SIZE, Globals::PICKUP_CUBE_FACE_SIZE) 
	};

	constexpr std::array<glm::vec3, 4> PICKUP_CUBE_FACE_TOP = 
	{ 
		glm::vec3(0, Globals::PICKUP_CUBE_FACE_SIZE, Globals::PICKUP_CUBE_FACE_SIZE), 
		glm::vec3(Globals::PICKUP_CUBE_FACE_SIZE, Globals::PICKUP_CUBE_FACE_SIZE, Globals::PICKUP_CUBE_FACE_SIZE), 
		glm::vec3(Globals::PICKUP_CUBE_FACE_SIZE, Globals::PICKUP_CUBE_FACE_SIZE, 0), 
		glm::vec3(0, Globals::PICKUP_CUBE_FACE_SIZE, 0) 
	};
	constexpr std::array<glm::vec3, 4> PICKUP_CUBE_FACE_BOTTOM = 
	{ 
		glm::vec3(0, 0, 0), 
		glm::vec3(Globals::PICKUP_CUBE_FACE_SIZE, 0, 0), 
		glm::vec3(Globals::PICKUP_CUBE_FACE_SIZE, 0, Globals::PICKUP_CUBE_FACE_SIZE), 
		glm::vec3(0, 0, Globals::PICKUP_CUBE_FACE_SIZE) 
	};

	constexpr std::array<glm::vec3, 4> PICKUP_FIRST_DIAGONAL_FACE = 
	{ 
		glm::vec3(0, 0, 0), 
		glm::vec3(Globals::PICKUP_CUBE_FACE_SIZE, 0, Globals::PICKUP_CUBE_FACE_SIZE), 
		glm::vec3(Globals::PICKUP_CUBE_FACE_SIZE, Globals::PICKUP_CUBE_FACE_SIZE, Globals::PICKUP_CUBE_FACE_SIZE), 
		glm::vec3(0, Globals::PICKUP_CUBE_FACE_SIZE, 0) 
	};
	constexpr std::array<glm::vec3, 4> PICKUP_SECOND_DIAGONAL_FACE = 
	{ 
		glm::vec3(0, 0, Globals::PICKUP_CUBE_FACE_SIZE), 
		glm::vec3(Globals::PICKUP_CUBE_FACE_SIZE, 0, 0), 
		glm::vec3(Globals::PICKUP_CUBE_FACE_SIZE, Globals::PICKUP_CUBE_FACE_SIZE, 0), 
		glm::vec3(0, Globals::PICKUP_CUBE_FACE_SIZE, Globals::PICKUP_CUBE_FACE_SIZE) 
	};

	void getTextCoords(std::vector<glm::vec3>& textCoords, eDestroyCubeIndex destroyCubeIndex)
	{
		glm::vec2 startingPosition = { 0.0f, 0.0f };
		glm::vec2 endingPosition = { 0.0f, 1.0f };
		constexpr float xOffset = 0.1f;

		switch (destroyCubeIndex)
		{
		case eDestroyCubeIndex::One:
			startingPosition.x = 0.0f;
			endingPosition.x = xOffset;
			break;
		case eDestroyCubeIndex::Two:
			startingPosition.x = xOffset;
			endingPosition.x = xOffset * 2;
			break;
		case eDestroyCubeIndex::Three:
			startingPosition.x = xOffset * 2;
			endingPosition.x = xOffset * 3;
			break;
		case eDestroyCubeIndex::Four:
			startingPosition.x = xOffset * 3;
			endingPosition.x = xOffset * 4;
			break;
		case eDestroyCubeIndex::Five:
			startingPosition.x = xOffset * 4;
			endingPosition.x = xOffset * 5;
			break;
		case eDestroyCubeIndex::Six:
			startingPosition.x = xOffset * 5;
			endingPosition.x = xOffset * 6;
			break;
		case eDestroyCubeIndex::Seven:
			startingPosition.x = xOffset * 6;
			endingPosition.x = xOffset * 7;
			break;
		case eDestroyCubeIndex::Eight:
			startingPosition.x = xOffset * 7;
			endingPosition.x = xOffset * 8;
			break;
		case eDestroyCubeIndex::Nine:
			startingPosition.x = xOffset * 8;
			endingPosition.x = xOffset * 9;
			break;
		case eDestroyCubeIndex::Ten:
			startingPosition.x = xOffset * 9;
			endingPosition.x = xOffset * 10;
			break;
		default:
			assert(false);
		}

		std::array<glm::vec3, 4> t = getDestroyBlockTextCoords(startingPosition, endingPosition);
		textCoords.insert(textCoords.end(), t.begin(), t.end());
	}

	void getTextCoords(std::vector<glm::vec3>& textCoords, eCubeSide cubeSide, eCubeType cubeType)
	{
		eTerrainTextureLayer textureLayer;
		switch (cubeType)
		{
		case eCubeType::Dirt:
			textureLayer = eTerrainTextureLayer::Dirt;
			break;
		case eCubeType::Stone:
			textureLayer = eTerrainTextureLayer::Stone;
			break;
		case eCubeType::Sand:
			textureLayer = eTerrainTextureLayer::Sand;
			break;
		case eCubeType::Grass:
			switch (cubeSide)
			{
			case eCubeSide::Back:
			case eCubeSide::Front:
			case eCubeSide::Left:
			case eCubeSide::Right:
				textureLayer = eTerrainTextureLayer::GrassSide;
				break;
			case eCubeSide::Top:
				textureLayer = eTerrainTextureLayer::Grass;
				break;
			case eCubeSide::Bottom:
				textureLayer = eTerrainTextureLayer::Dirt;
				break;
			}
			break;
		case eCubeType::Log:
			textureLayer = eTerrainTextureLayer::Log;
			break;
		case eCubeType::LogTop:
			switch (cubeSide)
			{
			case eCubeSide::Back:
			case eCubeSide::Front:
			case eCubeSide::Left:
			case eCubeSide::Right:
			case eCubeSide::Bottom:
				textureLayer = eTerrainTextureLayer::Log;
				break;
			case eCubeSide::Top:
				textureLayer = eTerrainTextureLayer::LogTop;
				break;
			}
			break;
		case eCubeType::Leaves:
			textureLayer = eTerrainTextureLayer::Leaves;
			break;
		case eCubeType::Cactus:
			textureLayer = eTerrainTextureLayer::Cactus;
			break;
		case eCubeType::CactusTop:
			switch (cubeSide)
			{
			case eCubeSide::Back:
			case eCubeSide::Front:
			case eCubeSide::Left:
			case eCubeSide::Right:
			case eCubeSide::Bottom:
				textureLayer = eTerrainTextureLayer::Cactus;
				break;
			case eCubeSide::Top:
				textureLayer = eTerrainTextureLayer::CactusTop;
				break;
			}
			break;
		case eCubeType::Water:
			textureLayer = eTerrainTextureLayer::Water;
			break;
		case eCubeType::Shrub:
			textureLayer = eTerrainTextureLayer::Shrub;
			break;
		case eCubeType::TallGrass:
			textureLayer = eTerrainTextureLayer::TallGrass;
			break;

		default:
			textureLayer = eTerrainTextureLayer::Error;
		}

		assert(textureLayer != eTerrainTextureLayer::Error);
		for (const auto& i : TEXT_COORDS)
		{
			textCoords.emplace_back(i.x, i.y, static_cast<int>(textureLayer));
		}
	}

	void addVoxelSelectionCubeFace(VertexBuffer& vertexBuffer, eCubeSide cubeSide, glm::vec3 cubePosition)
	{
		constexpr float offset = 0.005f;
		switch (cubeSide)
		{
		case eCubeSide::Front:
			for (const glm::vec3& position : CUBE_FACE_FRONT)
			{
				vertexBuffer.positions.push_back(cubePosition + glm::vec3(position.x, position.y, position.z + offset));
			}
			break;
		case eCubeSide::Back:
			for (const glm::vec3& position : CUBE_FACE_BACK)
			{
				vertexBuffer.positions.push_back(cubePosition + glm::vec3(position.x, position.y, position.z - offset));
			}
			break;
		case eCubeSide::Left:
			for (const glm::vec3& position : CUBE_FACE_LEFT)
			{
				vertexBuffer.positions.push_back(cubePosition + glm::vec3(position.x - offset, position.y, position.z));
			}
			break;
		case eCubeSide::Right:
			for (const glm::vec3& position : CUBE_FACE_RIGHT)
			{
				vertexBuffer.positions.push_back(cubePosition + glm::vec3(position.x + offset, position.y, position.z));
			}
			break;
		case eCubeSide::Top:
			for (const glm::vec3& position : CUBE_FACE_TOP)
			{
				vertexBuffer.positions.push_back(cubePosition + glm::vec3(position.x, position.y + offset, position.z));
			}
			break;
		case eCubeSide::Bottom:
			for (const glm::vec3& position : CUBE_FACE_BOTTOM)
			{
				vertexBuffer.positions.push_back(cubePosition + glm::vec3(position.x, position.y - offset, position.z));
			}
			break;
		default:
			assert(false);
		}

		for (const auto& i : TEXT_COORDS)
		{
			vertexBuffer.textCoords.emplace_back(i.x, i.y, 0.0f);
		}

		for (unsigned int i : CUBE_FACE_INDICIES)
		{
			vertexBuffer.indicies.emplace_back(i + vertexBuffer.elementBufferIndex);
		}

		vertexBuffer.elementBufferIndex += CUBE_FACE_INDICIE_COUNT;
	}

	void addDestroyBlockCubeFace(VertexBuffer& vertexBuffer, eCubeSide cubeSide, eDestroyCubeIndex destroyCubeIndex, glm::vec3 cubePosition)
	{
		constexpr float offset = 0.01f;
		switch (cubeSide)
		{
		case eCubeSide::Front:
			for (const glm::vec3& position : CUBE_FACE_FRONT)
			{
				vertexBuffer.positions.push_back(cubePosition + glm::vec3(position.x, position.y, position.z + offset));
			}
			break;
		case eCubeSide::Back:
			for (const glm::vec3& position : CUBE_FACE_BACK)
			{
				vertexBuffer.positions.push_back(cubePosition + glm::vec3(position.x, position.y, position.z - offset));
			}
			break;
		case eCubeSide::Left:
			for (const glm::vec3& position : CUBE_FACE_LEFT)
			{
				vertexBuffer.positions.push_back(cubePosition + glm::vec3(position.x - offset, position.y, position.z));
			}
			break;
		case eCubeSide::Right:
			for (const glm::vec3& position : CUBE_FACE_RIGHT)
			{
				vertexBuffer.positions.push_back(cubePosition + glm::vec3(position.x + offset, position.y, position.z + offset));
			}
			break;
		case eCubeSide::Top:
			for (const glm::vec3& position : CUBE_FACE_TOP)
			{
				vertexBuffer.positions.push_back(cubePosition + glm::vec3(position.x, position.y + offset, position.z + offset));
			}
			break;
		case eCubeSide::Bottom:
			for (const glm::vec3& position : CUBE_FACE_BOTTOM)
			{
				vertexBuffer.positions.push_back(cubePosition + glm::vec3(position.x, position.y - offset, position.z + offset));
			}
			break;
		default:
			assert(false);
		}

		getTextCoords(vertexBuffer.textCoords, destroyCubeIndex);

		for (unsigned int i : CUBE_FACE_INDICIES)
		{
			vertexBuffer.indicies.emplace_back(i + vertexBuffer.elementBufferIndex);
		}

		vertexBuffer.elementBufferIndex += CUBE_FACE_INDICIE_COUNT;
	}

	void addPickupCubeFace(VertexBuffer& vertexBuffer, eCubeType cubeType, eCubeSide cubeSide)
	{
		switch (cubeSide)
		{
		case eCubeSide::Front:
			for (const glm::vec3& position : PICKUP_CUBE_FACE_FRONT)
			{
				vertexBuffer.positions.push_back(position);
				vertexBuffer.lightIntensityVertices.push_back(FRONT_FACE_LIGHTING_INTENSITY);
			}

			break;
		case eCubeSide::Back:
			for (const glm::vec3& position : PICKUP_CUBE_FACE_BACK)
			{
				vertexBuffer.positions.push_back(position);
				vertexBuffer.lightIntensityVertices.push_back(BACK_FACE_LIGHTING_INTENSITY);
			}

			break;
		case eCubeSide::Left:
			for (const glm::vec3& position : PICKUP_CUBE_FACE_LEFT)
			{
				vertexBuffer.positions.push_back(position);
				vertexBuffer.lightIntensityVertices.push_back(LEFT_FACE_LIGHTING_INTENSITY);
			}
			break;
		case eCubeSide::Right:
			for (const glm::vec3& position : PICKUP_CUBE_FACE_RIGHT)
			{
				vertexBuffer.positions.push_back(position);
				vertexBuffer.lightIntensityVertices.push_back(RIGHT_FACE_LIGHTING_INTENSITY);
			}

			break;
		case eCubeSide::Top:
			for (const glm::vec3& position : PICKUP_CUBE_FACE_TOP)
			{
				vertexBuffer.positions.push_back(position);
				vertexBuffer.lightIntensityVertices.push_back(TOP_LIGHTING_INTENSITY);
			}
			break;
		case eCubeSide::Bottom:
			for (const glm::vec3& position : PICKUP_CUBE_FACE_BOTTOM)
			{
				vertexBuffer.positions.push_back(position);
				vertexBuffer.lightIntensityVertices.push_back(BOTTOM_FACE_LIGHTING_INTENSITY);
			}

			break;
		default:
			assert(false);
		}

		getTextCoords(vertexBuffer.textCoords, cubeSide, cubeType);

		for (unsigned int i : CUBE_FACE_INDICIES)
		{
			vertexBuffer.indicies.emplace_back(i + vertexBuffer.elementBufferIndex);
		}

		vertexBuffer.elementBufferIndex += CUBE_FACE_INDICIE_COUNT;
	}
}

void generateOuterChunkMesh(VertexArray& chunkMesh, const Chunk& chunk, const NeighbouringChunks& neighbouringChunks);
void generateInnerChunkMesh(VertexArray& chunkMesh, const Chunk& chunk);
void generateChunkInnerCubeMesh(const glm::ivec3& position, const Chunk& chunk, eCubeType cubeType, VertexArray& chunkMesh);
void generateChunkOuterCubeMesh(const glm::ivec3& position, const Chunk& chunk, const NeighbouringChunks& neighbouringChunks,
	eCubeType cubeType, VertexArray& chunkMesh);
void addCubeFace(VertexBuffer& vertexBuffer, eCubeType cubeType, eCubeSide cubeSide, const glm::vec3& cubePosition,
	bool transparent, bool shadow = false);
void addDiagonalCubeFace(VertexBuffer& vertexBuffer, eCubeType cubeType, const glm::ivec3& cubePosition,
	const std::array<glm::vec3, 4>& diagonalFace, bool shadow = false);

bool isFacingTransparentCube(const glm::ivec3& cubePosition, const Chunk& chunk);
bool isFacingOpaqueCube(const glm::ivec3& cubePosition, const Chunk& chunk);

void MeshGenerator::generateVoxelSelectionMesh(VertexBuffer& mesh, const glm::vec3& position)
{
	addVoxelSelectionCubeFace(mesh, eCubeSide::Left, position);
	addVoxelSelectionCubeFace(mesh, eCubeSide::Right, position);
	addVoxelSelectionCubeFace(mesh, eCubeSide::Top, position);
	addVoxelSelectionCubeFace(mesh, eCubeSide::Bottom, position);
	addVoxelSelectionCubeFace(mesh, eCubeSide::Front, position);
	addVoxelSelectionCubeFace(mesh, eCubeSide::Back, position);

	mesh.bindToVAO = true;
}

void MeshGenerator::generateDestroyBlockMesh(VertexBuffer& destroyBlockMesh, eDestroyCubeIndex destroyCubeIndex, const glm::vec3& position)
{
	addDestroyBlockCubeFace(destroyBlockMesh, eCubeSide::Left, destroyCubeIndex, position);
	addDestroyBlockCubeFace(destroyBlockMesh, eCubeSide::Right, destroyCubeIndex, position);
	addDestroyBlockCubeFace(destroyBlockMesh, eCubeSide::Top, destroyCubeIndex, position);
	addDestroyBlockCubeFace(destroyBlockMesh, eCubeSide::Bottom, destroyCubeIndex, position);
	addDestroyBlockCubeFace(destroyBlockMesh, eCubeSide::Front, destroyCubeIndex, position);
	addDestroyBlockCubeFace(destroyBlockMesh, eCubeSide::Back, destroyCubeIndex, position);

	destroyBlockMesh.bindToVAO = true;
}

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

void MeshGenerator::generatePickUpMesh(VertexBuffer& pickUpMesh, eCubeType cubeType)
{
	addPickupCubeFace(pickUpMesh, cubeType, eCubeSide::Left);
	addPickupCubeFace(pickUpMesh, cubeType, eCubeSide::Right);
	addPickupCubeFace(pickUpMesh, cubeType, eCubeSide::Top);
	addPickupCubeFace(pickUpMesh, cubeType, eCubeSide::Bottom);
	addPickupCubeFace(pickUpMesh, cubeType, eCubeSide::Front);
	addPickupCubeFace(pickUpMesh, cubeType, eCubeSide::Back);

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
			addCubeFace(chunkMesh.m_transparentVertexBuffer, cubeType, eCubeSide::Top, { position.x, position.y - WATER_OFFSET_Y, position.z }, true);
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
			addCubeFace(chunkMesh.m_transparentVertexBuffer, cubeType, eCubeSide::Top, { position.x, position.y - WATER_OFFSET_Y, position.z }, true);
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

void addCubeFace(VertexBuffer& vertexBuffer, eCubeType cubeType, eCubeSide cubeSide, const glm::vec3& cubePosition, bool transparent, bool shadow)
{
	glm::vec3 position = cubePosition;
	switch (cubeSide)
	{
	case eCubeSide::Front:
		for (const glm::vec3& i : CUBE_FACE_FRONT)
		{
			position += i;
			vertexBuffer.positions.emplace_back(position.x, position.y, position.z);
			position = cubePosition;

			if (shadow)
			{
				vertexBuffer.lightIntensityVertices.push_back(SHADOW_INTENSITY);
			}
			else if (transparent)
			{
				vertexBuffer.lightIntensityVertices.push_back(DEFAULT_LIGHTING_INTENSITY);
			}
			else
			{
				vertexBuffer.lightIntensityVertices.push_back(FRONT_FACE_LIGHTING_INTENSITY);
			}
		}

		break;
	case eCubeSide::Back:
		for (const glm::vec3& i : CUBE_FACE_BACK)
		{
			position += i;
			vertexBuffer.positions.emplace_back(position.x, position.y, position.z);
			position = cubePosition;

			if (shadow)
			{
				vertexBuffer.lightIntensityVertices.push_back(SHADOW_INTENSITY);
			}
			else if (transparent)
			{
				vertexBuffer.lightIntensityVertices.push_back(DEFAULT_LIGHTING_INTENSITY);
			}
			else
			{
				vertexBuffer.lightIntensityVertices.push_back(BACK_FACE_LIGHTING_INTENSITY);
			}
		}

		break;
	case eCubeSide::Left:
		for (const glm::vec3& i : CUBE_FACE_LEFT)
		{
			position += i;
			vertexBuffer.positions.emplace_back(position.x, position.y, position.z);
			position = cubePosition;

			if (shadow)
			{
				vertexBuffer.lightIntensityVertices.push_back(SHADOW_INTENSITY);
			}
			else if (transparent)
			{
				vertexBuffer.lightIntensityVertices.push_back(DEFAULT_LIGHTING_INTENSITY);
			}
			else
			{
				vertexBuffer.lightIntensityVertices.push_back(LEFT_FACE_LIGHTING_INTENSITY);
			}
		}

		break;
	case eCubeSide::Right:
		for (const glm::vec3& i : CUBE_FACE_RIGHT)
		{
			position += i;
			vertexBuffer.positions.emplace_back(position.x, position.y, position.z);
			position = cubePosition;

			if (shadow)
			{
				vertexBuffer.lightIntensityVertices.push_back(SHADOW_INTENSITY);
			}
			else if (transparent)
			{
				vertexBuffer.lightIntensityVertices.push_back(DEFAULT_LIGHTING_INTENSITY);
			}
			else
			{
				vertexBuffer.lightIntensityVertices.push_back(RIGHT_FACE_LIGHTING_INTENSITY);
			}
		}

		break;
	case eCubeSide::Top:
		for (const glm::vec3& i : CUBE_FACE_TOP)
		{
			position += i;
			vertexBuffer.positions.emplace_back(position.x, position.y, position.z);
			position = cubePosition;

			if (shadow)
			{
				vertexBuffer.lightIntensityVertices.push_back(SHADOW_INTENSITY);
			}
			else if (transparent)
			{
				vertexBuffer.lightIntensityVertices.push_back(DEFAULT_LIGHTING_INTENSITY);
			}
			else
			{
				vertexBuffer.lightIntensityVertices.push_back(TOP_LIGHTING_INTENSITY);
			}

		}

		break;
	case eCubeSide::Bottom:
		for (const glm::vec3& i : CUBE_FACE_BOTTOM)
		{
			position += i;
			vertexBuffer.positions.emplace_back(position.x, position.y, position.z);
			position = cubePosition;

			if (shadow)
			{
				vertexBuffer.lightIntensityVertices.push_back(SHADOW_INTENSITY);
			}
			else if (transparent)
			{
				vertexBuffer.lightIntensityVertices.push_back(DEFAULT_LIGHTING_INTENSITY);
			}
			else
			{
				vertexBuffer.lightIntensityVertices.push_back(BOTTOM_FACE_LIGHTING_INTENSITY);
			}
		}

		break;
	}

	getTextCoords(vertexBuffer.textCoords, cubeSide, cubeType);
	
	for (unsigned int i : CUBE_FACE_INDICIES)
	{
		vertexBuffer.indicies.emplace_back(i + vertexBuffer.elementBufferIndex);
	}

	vertexBuffer.elementBufferIndex += CUBE_FACE_INDICIE_COUNT;
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
			vertexBuffer.lightIntensityVertices.push_back(SHADOW_INTENSITY);
		}
		else
		{
			vertexBuffer.lightIntensityVertices.push_back(DEFAULT_LIGHTING_INTENSITY);
		}

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
