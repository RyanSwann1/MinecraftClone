#pragma once

#include "glm/glm.hpp"

//	//https://silentmatt.com/rectangle-intersection/
struct Rectangle
{
	Rectangle();
	Rectangle(const glm::vec2& position, float distance);

	bool contains(const glm::ivec2& position) const;
	bool contains(const Rectangle& other) const;

	void reset(const glm::vec2& position, float distance);

	float m_left;
	float m_right;
	float m_top;
	float m_bottom;
};