#pragma once

#include "glm/glm.hpp"

struct Rectangle
{
	Rectangle(glm::vec2 position, float distance)
		: left(position.x - distance),
		right(position.x + distance),
		top(position.y + distance),
		bottom(position.y - distance),
		distance(distance)
	{}

	void update(glm::vec2 position, float distance)
	{
		left = position.x - distance;
		right = position.x + distance;
		top = position.y + distance;
		bottom = position.y - distance;
	}

	bool contains(Rectangle rectangle) const
	{

	}

	float left;
	float right;
	float top;
	float bottom;
	float distance;
};