#pragma once

#include "glm/glm.hpp"
#include "CubeID.h"
#include "Utilities.h"
#include <array>

struct CubeDetails
{
	CubeDetails()
		: type(static_cast<char>(eCubeType::Invalid))
	{}

	CubeDetails(eCubeType type)
		: type(static_cast<char>(type))
	{}

	char type;
};

//position.y * (CHUNK_AREA) + position.z * CHUNK_SIZE + position.x;
class Chunk
{
public:
	Chunk();
	Chunk(const glm::ivec3& startingPosition);
	
	bool isInUse() const;
	bool isPositionInBounds(const glm::ivec3& position) const;
	const glm::ivec3& getStartingPosition() const;
	const CubeDetails& getCubeDetailsWithoutBoundsCheck(const glm::ivec3& position) const;

	Chunk* getNext();
	void setNext(Chunk* chunk);
	void reuse(const glm::ivec3& startingPosition);
	void release();

private:
	bool m_inUse;

	glm::ivec3 m_startingPosition;
	glm::ivec3 m_endingPosition;
	std::array<std::array<std::array<CubeDetails, Utilities::CHUNK_DEPTH>, Utilities::CHUNK_HEIGHT>, Utilities::CHUNK_WIDTH> m_chunk;
	Chunk* m_next;

	//union State
	//{
	//	State()
	//		: m_startingPosition(),
	//		m_endingPosition(),
	//		m_chunk()
	//	{}

	//	struct
	//	{
	//		glm::ivec3 m_startingPosition;
	//		glm::ivec3 m_endingPosition;
	//		std::array<std::array<std::array<CubeDetails, Utilities::CHUNK_DEPTH>, Utilities::CHUNK_HEIGHT>, Utilities::CHUNK_WIDTH> m_chunk;
	//	};

	//	Chunk* m_next = nullptr;
	//} m_state;

	void regen(const glm::ivec3& startingPosition);
};