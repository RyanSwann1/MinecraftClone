#include "Chunk.h"
#include "VertexBuffer.h"
#include "VertexBuffer.h"
#include "ChunkManager.h"
#include "NeighbouringChunks.h"
#include "glm/gtc/noise.hpp"
#include <algorithm>
#include <limits>

//OLD VALUES

//MAP_SIZE == 8000;
//double ex = (x) / (Utilities::MAP_SIZE * 1.0f) - 0.5f;
//double ey = (z) / (Utilities::MAP_SIZE * 1.0f) - 0.5f;
//
//float elevation = std::abs(1 * glm::perlin(glm::vec2(5.0f * ex, 5.0f * ey)));
//elevation += std::abs(0.5 * glm::perlin(glm::vec2(ex * 15.0f, ey * 15.0f)));
//elevation += std::abs(0.25 * glm::perlin(glm::vec2(ex * 30.0f, ey * 30.0f)));
//
//elevation = glm::pow(elevation, 2.5f);
//elevation = elevation * (float)Utilities::CHUNK_HEIGHT;
//elevation = Utilities::clampTo(elevation, 0.0f, (float)Utilities::CHUNK_HEIGHT - 1.0f);
//
//double mx = (x) / (Utilities::MAP_SIZE * 1.0f) - 0.5f;
//double my = (z) / (Utilities::MAP_SIZE * 1.0f) - 0.5f;
//float moisture = std::abs(1 * glm::perlin(glm::vec2(15.0f * mx, 15.0f * my)));

namespace
{
	constexpr int MAX_LEAVES_DISTANCE = 3;
	constexpr std::array<int, 6> LEAVES_DISTANCES =
	{
		MAX_LEAVES_DISTANCE,
		MAX_LEAVES_DISTANCE,
		MAX_LEAVES_DISTANCE - 1,
		MAX_LEAVES_DISTANCE - 1,
		MAX_LEAVES_DISTANCE - 2,
		MAX_LEAVES_DISTANCE - 2
	};

	int converTo1D(const glm::ivec3& position)
	{
		return (position.z * Globals::CHUNK_WIDTH * Globals::CHUNK_HEIGHT) + (position.y * Globals::CHUNK_WIDTH) + position.x;
	}

	glm::ivec3 convertTo3D(int idx)
	{
		glm::ivec3 position;
		position.z = idx / (Globals::CHUNK_WIDTH * Globals::CHUNK_HEIGHT);
		idx -= (position.z * Globals::CHUNK_WIDTH * Globals::CHUNK_HEIGHT);
		position.y = idx / Globals::CHUNK_WIDTH;
		position.x = idx % Globals::CHUNK_WIDTH;
		return position;
	}

	glm::ivec3 convertToLocalPosition(const glm::ivec3& worldPosition, const glm::ivec3& chunkStartingPosition)
	{
		return { worldPosition.x - chunkStartingPosition.x, worldPosition.y - chunkStartingPosition.y, worldPosition.z - chunkStartingPosition.z };
	}

	const CubeTypeComparison NON_STACKABLE_CUBE_TYPES =
	{
		{eCubeType::Water,
		eCubeType::Shrub,
		eCubeType::TallGrass}
	};

	static int SEED = Globals::getRandomNumber(0, Globals::MAP_SIZE);
}

Chunk::Chunk()
	: m_startingPosition(),
	m_endingPosition(),
	m_chunk(),
	m_AABB()
{}

Chunk::Chunk(const glm::ivec3& startingPosition)
	: m_startingPosition(startingPosition),
	m_endingPosition(startingPosition.x + Globals::CHUNK_WIDTH, 
		startingPosition.y + Globals::CHUNK_HEIGHT, 
		startingPosition.z + Globals::CHUNK_DEPTH),
	m_chunk(),
	m_AABB(glm::ivec2(m_startingPosition.x, m_startingPosition.z) +
		glm::ivec2(Globals::CHUNK_WIDTH / 2, Globals::CHUNK_DEPTH / 2), 16)
{
	regen(m_startingPosition);
}

Chunk::Chunk(Chunk&& orig) noexcept
	: m_startingPosition(orig.m_startingPosition),
	m_endingPosition(orig.m_endingPosition),
	m_chunk(std::move(orig.m_chunk)),
	m_AABB(orig.m_AABB)
{}

Chunk& Chunk::operator=(Chunk&& orig) noexcept
{
	m_startingPosition = orig.m_startingPosition;
	m_endingPosition = orig.m_endingPosition;
	m_chunk = std::move(orig.m_chunk);
	m_AABB = orig.m_AABB;

	return *this;
}

glm::ivec3 Chunk::getHighestCubeAtPosition(const glm::ivec3& position) const
{
	for (int y = Globals::CHUNK_HEIGHT - 1; y >= 0; --y)
	{
		if (isCubeAtLocalPosition(convertToLocalPosition({ position.x, y, position.z }, m_startingPosition)))
		{
			return { position.x, y + 1, position.z };
		}
	}

	assert(false);
	return glm::ivec3(position.x, Globals::CHUNK_HEIGHT - 1, position.z);
}

bool Chunk::isCubeBelowCovering(const glm::ivec3& startingPosition) const
{
	for (int y = startingPosition.y + 1; 
		y <= startingPosition.y + Globals::MAX_SHADOW_HEIGHT && y < Globals::CHUNK_HEIGHT - 1; ++y)
	{
		eCubeType cubeAtPosition = static_cast<eCubeType>(getCubeDetailsWithoutBoundsCheck({ startingPosition.x, y, startingPosition.z }));
		if (cubeAtPosition == eCubeType::Leaves)
		{
			return true;
		}
	}

	return false;
}

const Rectangle& Chunk::getAABB() const
{
	return m_AABB;
}

bool Chunk::isPositionInBounds(const glm::ivec3& position) const
{
	return (position.x >= m_startingPosition.x &&
		position.y >= m_startingPosition.y &&
		position.z >= m_startingPosition.z &&
		position.x <= m_endingPosition.x - 1 &&
		position.y <= m_endingPosition.y - 1 &&
		position.z <= m_endingPosition.z - 1);
}

const glm::ivec3& Chunk::getStartingPosition() const
{
	return m_startingPosition;
}

const glm::ivec3& Chunk::getEndingPosition() const
{
	return m_endingPosition;
}

char Chunk::getCubeDetailsWithoutBoundsCheck(const glm::ivec3& position) const
{
	glm::ivec3 positionOnGrid(position.x - m_startingPosition.x, position.y - m_startingPosition.y, position.z - m_startingPosition.z);
	assert(isPositionInLocalBounds(positionOnGrid));
	return m_chunk[converTo1D(positionOnGrid)];
}

bool Chunk::isCubeAtPosition(const glm::ivec3& position) const
{
	if (isPositionInBounds(position))
	{
		return m_chunk[converTo1D({ position.x - m_startingPosition.x, position.y - m_startingPosition.y,
			position.z - m_startingPosition.z })] != static_cast<char>(eCubeType::Air);
	}

	return false;
}

bool Chunk::isCubeAtPosition(const glm::ivec3& position, eCubeType cubeType) const
{
	if (isPositionInBounds(position))
	{
		return m_chunk[converTo1D({ position.x - m_startingPosition.x, position.y - m_startingPosition.y,
			position.z - m_startingPosition.z })] == static_cast<char>(cubeType);
	}

	return false;
}

bool Chunk::isCubeAtLocalPosition(const glm::ivec3& localPosition) const
{
	assert(isPositionInLocalBounds(localPosition));
	return m_chunk[converTo1D(localPosition)] != static_cast<char>(eCubeType::Air);
}

void Chunk::changeCubeAtLocalPosition(const glm::ivec3& position, eCubeType cubeType)
{
	assert(isPositionInLocalBounds(position));
	m_chunk[converTo1D(position)] = static_cast<char>(cubeType);
}

bool Chunk::addCubeAtPosition(const glm::ivec3& placementPosition, const NeighbouringChunks& neighbouringChunks, eCubeType cubeType)
{
	glm::ivec3 localPosition = convertToLocalPosition(placementPosition, m_startingPosition);
	assert(isPositionInLocalBounds(localPosition));

	if (localPosition.y < Globals::CHUNK_HEIGHT && !isCubeAtLocalPosition(localPosition))
	{
		if (isCubeAtLocalPosition({ localPosition.x, localPosition.y - 1, localPosition.z }) &&
			!NON_STACKABLE_CUBE_TYPES.isMatch(getCubeTypeByLocalPosition({ localPosition.x, localPosition.y - 1, localPosition.z })))
		{
			changeCubeAtLocalPosition(localPosition, cubeType);
			return true;
		}
		else
		{
			//Search local bounds
			for (int z = localPosition.z - 1; z <= localPosition.z + 1; z += 2)
			{
				if (isPositionInLocalBounds({ localPosition.x, localPosition.y, z }) && isCubeAtLocalPosition({ localPosition.x, localPosition.y, z }))
				{
					changeCubeAtLocalPosition(localPosition, cubeType);
					return true;
				}
			}

			for (int x = localPosition.x - 1; x <= localPosition.x + 1; x += 2)
			{
				if (isPositionInLocalBounds({ x, localPosition.y, localPosition.z }) && isCubeAtLocalPosition({ x, localPosition.y, localPosition.z }))
				{
					changeCubeAtLocalPosition(localPosition, cubeType);
					return true;
				}
			}

			//Search Neighbouring Chunks
			for (const auto& neighbouringChunk : neighbouringChunks.chunks)
			{
				for (int z = placementPosition.z - 1; z <= placementPosition.z + 1; z += 2)
				{
					if (neighbouringChunk.get().isPositionInBounds({ placementPosition.x, placementPosition.y, z }) &&
						neighbouringChunk.get().isCubeAtPosition({ placementPosition.x, placementPosition.y, z }))
					{
						changeCubeAtLocalPosition(localPosition, cubeType);
						return true;
					}
				}

				for (int x = placementPosition.x - 1; x <= placementPosition.x + 1; x += 2)
				{
					if (neighbouringChunk.get().isPositionInBounds({ x, placementPosition.y, placementPosition.z }) &&
						neighbouringChunk.get().isCubeAtPosition({ x, placementPosition.y, placementPosition.z }))
					{
						changeCubeAtLocalPosition(localPosition, cubeType);
						return true;
					}
				}
			}
		}
	}

	return false;
}

bool Chunk::destroyCubeAtPosition(const glm::ivec3& destroyPosition, eCubeType& destroyedCubeType)
{
	glm::ivec3 localPosition = convertToLocalPosition(destroyPosition, m_startingPosition);
	if (!isCubeAtLocalPosition(localPosition, eCubeType::Water) && !isCubeAtLocalPosition(localPosition, eCubeType::Air))
	{
		//Destroy specific Cubes on top
		if (localPosition.y < Globals::CHUNK_HEIGHT - 1 &&
			isCubeAtLocalPosition({ localPosition.x, localPosition.y + 1, localPosition.z }, eCubeType::TallGrass) ||
			isCubeAtLocalPosition({ localPosition.x, localPosition.y + 1, localPosition.z }, eCubeType::Shrub))
		{
			changeCubeAtLocalPosition({ localPosition.x, localPosition.y + 1, localPosition.z }, eCubeType::Air);
		}

		destroyedCubeType = getCubeTypeByLocalPosition(localPosition);
		changeCubeAtLocalPosition(localPosition, eCubeType::Air);
		return true;
	}

	return false;
}

void Chunk::reset()
{
	m_startingPosition = glm::ivec3();
	m_endingPosition = glm::ivec3();

	m_AABB = Rectangle();
}

void Chunk::reuse(const glm::ivec3& startingPosition)
{	
	memset(&m_chunk, static_cast<char>(eCubeType::Air), m_chunk.size());

	m_startingPosition = startingPosition;
	m_endingPosition = glm::ivec3(startingPosition.x + Globals::CHUNK_WIDTH, startingPosition.y + Globals::CHUNK_HEIGHT,
		startingPosition.z + Globals::CHUNK_DEPTH);
	m_AABB.reset(glm::ivec2(m_startingPosition.x, m_startingPosition.z) +
		glm::ivec2(Globals::CHUNK_WIDTH / 2, Globals::CHUNK_DEPTH / 2), 16);

	regen(m_startingPosition);	
}


void Chunk::regen(const glm::ivec3& startingPosition)
{
	for (int z = startingPosition.z; z < startingPosition.z + Globals::CHUNK_DEPTH; ++z)
	{
		for (int x = startingPosition.x; x < startingPosition.x + Globals::CHUNK_WIDTH; ++x)
		{
			int elevation = getElevationAtPosition(x, z);
			glm::ivec3 positionOnGrid(x - startingPosition.x, elevation, z - startingPosition.z);
			eCubeType cubeType;

			switch (getBiomeType(x, z))
			{
			case eBiomeType::Plains :
			{
				bool firstBlockPlaced = false;
				for (int y = elevation; y >= 0; --y)
				{
					if (y <= Globals::STONE_MAX_HEIGHT)
					{
						cubeType = eCubeType::Stone;
					}
					else
					{
						if (firstBlockPlaced)
						{
							cubeType = eCubeType::Dirt;
						}
						else
						{
							cubeType = eCubeType::Grass;
							firstBlockPlaced = true;
						}
					}

					changeCubeAtLocalPosition({ positionOnGrid.x, y, positionOnGrid.z }, cubeType);
				}
			}
			break;
			case eBiomeType::Desert:
			{
				for (int y = elevation; y >= 0; --y)
				{
					if (y <= Globals::STONE_MAX_HEIGHT)
					{
						cubeType = eCubeType::Stone;
					}
					else
					{
						cubeType = eCubeType::Sand;
					}

					changeCubeAtLocalPosition({ positionOnGrid.x, y, positionOnGrid.z }, cubeType);
				}
			}
			break;
			default:
				assert(false);
			}
		}
	}

	//Spawn Decoration
	spawnWater();
	spawnTrees();
	spawnCactus();
	spawnPlant(Globals::MAX_SHRUB_PER_CHUNK, eCubeType::Sand, eCubeType::Shrub);
	spawnPlant(Globals::MAX_TALL_GRASS_PER_CHUNK, eCubeType::Grass, eCubeType::TallGrass);
}

void Chunk::spawnWater()
{
	for (int z = 0; z < Globals::CHUNK_DEPTH; ++z)
	{
		for (int x = 0; x < Globals::CHUNK_WIDTH; ++x)
		{
			for (int y = 0; y <= Globals::WATER_MAX_HEIGHT; ++y)
			{
				glm::ivec3 position(x, y, z);
				if ((isCubeAtLocalPosition(position, eCubeType::Air)))
				{
					changeCubeAtLocalPosition(position, eCubeType::Water);
				}
			}
		}
	}
}

void Chunk::spawnTrees()
{
	if (Globals::getRandomNumber(0, 100) < Globals::CHANCE_TREE_SPAWN_IN_CHUNK)
	{
		return;
	}
	
	int attemptsCount = 0;
	int spawnCount = 0;
	while (attemptsCount < Globals::MAX_TREE_SPAWN_ATTEMPTS && spawnCount < Globals::MAX_TREE_PER_CHUNK)
	{
		glm::ivec3 spawnPosition;
		spawnPosition.x = Globals::getRandomNumber(MAX_LEAVES_DISTANCE, Globals::CHUNK_WIDTH - MAX_LEAVES_DISTANCE - 1);
		spawnPosition.z = Globals::getRandomNumber(MAX_LEAVES_DISTANCE, Globals::CHUNK_DEPTH - MAX_LEAVES_DISTANCE - 1);

		//Find Spawn Location
		for (int y = Globals::CHUNK_HEIGHT - Globals::MAX_TREE_HEIGHT - MAX_LEAVES_DISTANCE - 1; y >= Globals::SAND_MAX_HEIGHT; --y)
		{
			spawnPosition.y = y;
			if (isCubeAtLocalPosition(spawnPosition, eCubeType::Grass) &&
				isCubeAtLocalPosition({ spawnPosition.x, spawnPosition.y + 1, spawnPosition.z }, eCubeType::Air))
			{
				int treeHeight = Globals::getRandomNumber(Globals::MIN_TREE_HEIGHT, Globals::MAX_TREE_HEIGHT);
				spawnLeaves(spawnPosition, treeHeight);
				spawnTreeStump(spawnPosition, treeHeight);
				++spawnCount;
				break;
			}
		}

		++attemptsCount;
	}
}

void Chunk::spawnCactus()
{
	int attemptsCount = 0;
	int spawnCount = 0;
	while (attemptsCount < Globals::MAX_CACTUS_SPAWN_ATTEMPTS && spawnCount < Globals::MAX_CACTUS_PER_CHUNK)
	{
		glm::ivec3 spawnPosition;
		spawnPosition.x = Globals::getRandomNumber(0, Globals::CHUNK_WIDTH - 1);
		spawnPosition.z = Globals::getRandomNumber(0, Globals::CHUNK_DEPTH - 1);

		//Find Spawn Location
		for (int y = Globals::CHUNK_HEIGHT - Globals::CACTUS_MAX_HEIGHT - 1; y >= 0; --y)
		{
			spawnPosition.y = y;
			if (isCubeAtLocalPosition(spawnPosition, eCubeType::Sand) &&
				isCubeAtLocalPosition({ spawnPosition.x, y + 1, spawnPosition.z }, eCubeType::Air))
			{
				//Spawn Cactus
				int cactusHeight = Globals::getRandomNumber(Globals::CACTUS_MIN_HEIGHT, Globals::CACTUS_MAX_HEIGHT);
				for (int i = 1; i <= cactusHeight; ++i)
				{
					if (i == cactusHeight)
					{
						changeCubeAtLocalPosition({ spawnPosition.x, spawnPosition.y + i, spawnPosition.z }, eCubeType::CactusTop);
					}
					else
					{
						changeCubeAtLocalPosition({ spawnPosition.x, spawnPosition.y + i, spawnPosition.z }, eCubeType::Cactus);
					}
				}

				++spawnCount;
				break;
			}
		}

		++attemptsCount;
	}
}

void Chunk::spawnPlant(int maxQuantity, eCubeType baseCubeType, eCubeType plantCubeType)
{
	int attemptsCount = 0;
	int spawnCount = 0;
	while (attemptsCount < Globals::MAX_PLANT_SPAWN_ATTEMPTS && spawnCount < maxQuantity)
	{
		glm::ivec3 spawnPosition;
		spawnPosition.x = Globals::getRandomNumber(0, Globals::CHUNK_WIDTH - 1);
		spawnPosition.z = Globals::getRandomNumber(0, Globals::CHUNK_DEPTH - 1);

		for (int y = Globals::CHUNK_HEIGHT - 5; y >= Globals::WATER_MAX_HEIGHT; --y)
		{
			spawnPosition.y = y;
			if (isCubeAtLocalPosition(spawnPosition, eCubeType::Air) &&
				isCubeAtLocalPosition({ spawnPosition.x, y - 1, spawnPosition.z }, baseCubeType))
			{
				changeCubeAtLocalPosition(spawnPosition, plantCubeType);
				++spawnCount;
			}
		}

		++attemptsCount;
	}
}

void Chunk::spawnLeaves(const glm::ivec3& startingPosition, int treeHeight)
{
	int y = startingPosition.y + treeHeight / 2; //Starting Height
	for (int distance : LEAVES_DISTANCES)
	{
		++y;
		for (int z = startingPosition.z - distance; z <= startingPosition.z + distance; ++z)
		{
			for (int x = startingPosition.x - distance; x <= startingPosition.x + distance; ++x)
			{
				glm::ivec3 position(x, y, z);
				if (isCubeAtLocalPosition(position, eCubeType::Air))
				{
					changeCubeAtLocalPosition(position, eCubeType::Leaves);
				}
			}
		}
	}
}

void Chunk::spawnTreeStump(const glm::ivec3& startingPosition, int treeHeight)
{
	for (int y = startingPosition.y; y <= startingPosition.y + treeHeight / 2; ++y)
	{
		if (y == startingPosition.y + treeHeight / 2)
		{
			changeCubeAtLocalPosition({ startingPosition.x, y, startingPosition.z }, eCubeType::LogTop);
		}
		else
		{
			changeCubeAtLocalPosition({ startingPosition.x, y, startingPosition.z }, eCubeType::Log);
		}
	}
}

eCubeType Chunk::getCubeTypeByLocalPosition(const glm::ivec3& localPosition) const
{
	assert(isPositionInLocalBounds(localPosition));
	return static_cast<eCubeType>(m_chunk[converTo1D(localPosition)]);
}

bool Chunk::isPositionInLocalBounds(const glm::ivec3& position) const
{
	return (position.x >= 0 &&
		position.y >= 0 &&
		position.z >= 0 &&
		position.x < Globals::CHUNK_WIDTH &&
		position.y < Globals::CHUNK_HEIGHT &&
		position.z < Globals::CHUNK_DEPTH);
}

bool Chunk::isCubeAtLocalPosition(const glm::ivec3& localPosition, eCubeType cubeType) const
{
	assert(isPositionInLocalBounds(localPosition));
	return m_chunk[converTo1D(localPosition)] == static_cast<char>(cubeType);
}

int Chunk::getElevationAtPosition(int x, int z) const
{
	double ex = (x + SEED) / static_cast<float>(Globals::MAP_SIZE);
	double ey = (z + SEED) / static_cast<float>(Globals::MAP_SIZE);

	float elevation = 0.0f;

	float persistence = Globals::TERRAIN_PERSISTENCE;
	float lacunarity = Globals::TERRAIN_LACUNARITY;
	for (int i = 0; i < Globals::TERRAIN_OCTAVES; ++i)
	{
		elevation += persistence * glm::perlin(glm::vec2((ex * 1.5f)* lacunarity, (ey * 1.5f) * lacunarity));

		persistence /= 2.0f;
		lacunarity *= 2.0f;
	}
	
	persistence = Globals::TERRAIN_PERSISTENCE;
	float total = 0.0f;
	for (int i = 0; i < Globals::TERRAIN_OCTAVES; ++i)
	{
		total += persistence;
		persistence /= 2.0f;
	}

	
	elevation /= total;
	elevation = (elevation + 1) / 2;
	elevation = glm::pow(elevation, 3.0f);
	elevation = elevation * (float)Globals::CHUNK_HEIGHT - 1;

	return static_cast<int>(elevation);
}

//elevation = (elevation - -1) / (1 - -1) * (Utilities::CHUNK_HEIGHT - 1) + 1;

eBiomeType Chunk::getBiomeType(int x, int z) const
{
	double bx = x / 1000.0f;
	double by = z / 1000.0f;

	float biomeType = 0.0f;
	float moisturePersistence = Globals::BIOME_PERSISTENCE;
	float moistureLacunarity = Globals::BIOME_LACUNARITY;
	for (int i = 0; i < Globals::BIOME_OCTAVES; ++i)
	{
		biomeType += moisturePersistence * glm::perlin(glm::vec2(bx * moistureLacunarity, by * moistureLacunarity));

		moisturePersistence /= 2.0f;
		moistureLacunarity *= 2.0f;
	}

	float persistence = Globals::BIOME_PERSISTENCE;
	float total = 0.0f;
	for (int i = 0; i < Globals::BIOME_OCTAVES; ++i)
	{
		total += persistence;
		persistence /= 2.0f;
	}

	biomeType /= total;
	biomeType = (biomeType + 1) / 2;

	if (biomeType >= 0.4f)
	{
		return eBiomeType::Plains;
	}
	else
	{
		return eBiomeType::Desert;
	}
}