#include "Rectangle.h"
#include "Utilities.h"

Rectangle::Rectangle()
	: m_left(0.0f),
	m_right(0.0f),
	m_top(0.0f),
	m_bottom(0.0f),
	m_distance(0.0f)
{}

Rectangle::Rectangle(const glm::vec2& position, float distance)
	: m_left(position.x - distance),
	m_right(position.x + distance),
	m_top(position.y + distance),
	m_bottom(position.y - distance),
	m_distance(distance)
{}

void Rectangle::update(const glm::vec2 & position, float distance)
{
	m_left = position.x - distance;
	m_right = position.x + distance;
	m_top = position.y + distance;
	m_bottom = position.y - distance;
}

bool Rectangle::contains(const Rectangle& other) const
{
	return m_left <= other.m_right &&
		m_right >= other.m_left &&
		m_top >= other.m_bottom &&
		m_bottom <= other.m_top;
}


//Rectangle chunkAABB(glm::ivec2(chunk->second.chunk.getStartingPosition().x, chunk->second.chunk.getStartingPosition().z) +
//	glm::ivec2(Utilities::CHUNK_WIDTH / 2.0f, Utilities::CHUNK_DEPTH / 2.0f), 16);

void Rectangle::reset(const glm::vec2& position, float distance)
{
	m_left = position.x - distance;
	m_right = position.x + distance;
	m_top = position.y + distance;
	m_bottom = position.y - distance;
	m_distance = distance;
}
