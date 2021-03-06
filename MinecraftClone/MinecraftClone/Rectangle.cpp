#include "Rectangle.h"
#include "Globals.h"

Rectangle::Rectangle()
	: m_left(0.0f),
	m_right(0.0f),
	m_top(0.0f),
	m_bottom(0.0f)
{}

Rectangle::Rectangle(const glm::vec2& position, float distance)
	: m_left(position.x - distance),
	m_right(position.x + distance),
	m_top(position.y + distance),
	m_bottom(position.y - distance)
{}

bool Rectangle::contains(const glm::ivec2& position) const
{
	return position.x >= m_left &&
		position.x <= m_right &&
		position.y <= m_top &&
		position.y >= m_bottom;
}

bool Rectangle::contains(const Rectangle& other) const
{
	return m_left <= other.m_right &&
		m_right >= other.m_left &&
		m_top >= other.m_bottom &&
		m_bottom <= other.m_top;
}

void Rectangle::reset(const glm::vec2& position, float distance)
{
	m_left = position.x - distance;
	m_right = position.x + distance;
	m_top = position.y + distance;
	m_bottom = position.y - distance;
}
