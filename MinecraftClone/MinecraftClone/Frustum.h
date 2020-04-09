#pragma once

#include "glm/glm.hpp"
#include <array>

struct Frustum
{
	enum ePlaneSide
	{
		Near,
		Far,
		Left,
		Right,
		Top,
		Bottom,
		Max
	};

	struct Plane
	{
		float d;
		glm::vec3 n;
	};
	
	void update(const glm::mat4& mat);

	bool isChunkInFustrum(const glm::vec3& chunkStartingPosition) const;
	std::array<Plane, static_cast<int>(ePlaneSide::Max)> m_planes;
};