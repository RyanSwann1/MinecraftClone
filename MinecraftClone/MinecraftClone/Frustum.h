#pragma once

#include "NonCopyable.h"
#include "NonMovable.h"
#include "glm/glm.hpp"
#include <array>

class Frustum : private NonCopyable, private NonMovable
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
		Plane();

		float d;
		glm::vec3 n;
	};

public:
	Frustum();

	void update(const glm::mat4& mat);

	bool isChunkInFustrum(const glm::vec3& chunkStartingPosition) const;
	
private:
	std::array<Plane, static_cast<int>(ePlaneSide::Max)> m_planes;
};