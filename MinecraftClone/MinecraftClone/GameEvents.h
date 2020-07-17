#pragma once

#include "Globals.h"

namespace GameEvents
{
	struct SpawnPickUp
	{
		SpawnPickUp(eCubeType type, const glm::vec3& position)
			: type(type),
			position(position)
		{}

		eCubeType type;
		glm::vec3 position;
	};
}