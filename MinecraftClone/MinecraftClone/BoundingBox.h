#pragma once

#include "glm/glm.hpp"

struct BoundingBox
{
	BoundingBox(const glm::vec3& position, const glm::vec3& size)
		: minX(position.x - size.x),
		maxX(position.x + size.x),
		minY(position.y - size.y),
		maxY(position.y + size.y),
		minZ(position.z - size.z),
		maxZ(position.z + size.z)
	{}

	bool isIntersecting(const BoundingBox& other) const
	{
		return (maxX > other.minX &&
			minX < other.maxX&&
			maxY > other.minY &&
			minY < other.maxY&&
			maxZ > other.minZ &&
			minZ < other.maxZ);

		//return(tBox1.m_vecMax.x > tBox2.m_vecMin.x &&
		//	tBox1.m_vecMin.x < tBox2.m_vecMax.x&&
		//	tBox1.m_vecMax.y > tBox2.m_vecMin.y &&
		//	tBox1.m_vecMin.y < tBox2.m_vecMax.y&&
		//	tBox1.m_vecMax.z > tBox2.m_vecMin.z &&
		//	tBox1.m_vecMin.z < tBox2.m_vecMax.z);
	}

	float minX = 0;
	float maxX = 0;
	float minY = 0;
	float maxY = 0;
	float minZ = 0;
	float maxZ = 0;
};