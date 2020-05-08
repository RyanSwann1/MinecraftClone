#pragma once

#include "glm/glm.hpp"
#include "CubeID.h"
#include "Utilities.h"
#include "Rectangle.h"
#include "NonCopyable.h"
#include <array>

//Trees
//https://www.reddit.com/r/proceduralgeneration/comments/72waky/how_does_layered_generation_work_when_generating/
//https://www.reddit.com/r/proceduralgeneration/comments/2t2dyy/trees_in_minecraft_like_worlds_help/

//Saving/Storing
//https://www.reddit.com/r/proceduralgeneration/comments/3gwbux/question_how_does_the_world_remember_changes/

struct HeightMapNode
{
	HeightMapNode()
		:height(0),
		biomeTransition(false)
	{}

	HeightMapNode(int height)
		: height(height),
		biomeTransition(false)
	{}

	int height;
	bool biomeTransition;
};

enum class eTransitionType
{

};

enum class eBiomeType
{
	Plains = 0,
	Desert,
	Mountains,
	Islands
};

//position.y * (CHUNK_AREA) + position.z * CHUNK_SIZE + position.x;
class Chunk : private NonCopyable
{
public:
	Chunk();
	Chunk(const glm::ivec3& startingPosition);
	Chunk(Chunk&&) noexcept;
	Chunk& operator=(Chunk&&) noexcept;

	bool isCubeBelowCovering(const glm::ivec3& position) const;
	const Rectangle& getAABB() const;
	bool isPositionInBounds(const glm::ivec3& position) const;
	const glm::ivec3& getStartingPosition() const;
	const glm::ivec3& getEndingPosition() const;
	char getCubeDetailsWithoutBoundsCheck(const glm::ivec3& position) const;
	
	void reset();
	void reuse(const glm::ivec3& startingPosition);

private:
	glm::ivec3 m_startingPosition;
	glm::ivec3 m_endingPosition;
	std::array<std::array<HeightMapNode, 32>, 32> m_heightMap;
	std::array<char, Utilities::CHUNK_VOLUME> m_chunk;
	Rectangle m_AABB;

	bool isPositionInLocalBounds(const glm::ivec3& position) const;
	bool isCubeAtLocalPosition(const glm::ivec3& position, eCubeType cubeType) const;
	int getElevationAtPosition(int x, int y, float terrainLacunarity, float terrainPersistence, 
		float terrainOctaves, float terrainRedistribution, int maxHeight) const;
	eBiomeType getBiomeType(int x, int y) const;
	
	void changeCubeAtLocalPosition(const glm::ivec3& position, eCubeType cubeType);
	void regen(const glm::ivec3& startingPosition);
	void spawnWater();
	void spawnTrees();
	void spawnCactus();
	void spawnPlant(int maxQuantity, eCubeType baseCubeType, eCubeType plantCubeType);
	void spawnLeaves(const glm::ivec3& startingPosition, int treeHeight);
	void spawnTreeStump(const glm::ivec3& startingPosition, int treeHeight);

	void constructHeightMap(const glm::ivec2& startingPositionOnGrid);
	int getElevationAtPosition(const glm::ivec2& positionOnGrid) const;
	bool isPositionOnBiomeTransition(const glm::ivec2& position, int searchRadius) const;
	int addNoiseToPosition(int height);
};