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

		const eCubeType type;
		const glm::vec3 position;
	};

	struct PlayerDisgardPickup
	{
		PlayerDisgardPickup(eCubeType cubeType, const glm::vec3& position, const glm::vec3& initialVelocity)
			: cubeType(cubeType),
			position(position),
			initialVelocity(initialVelocity)
		{}
		
		const eCubeType cubeType;
		const glm::vec3 position;
		const glm::vec3 initialVelocity;
	};

	struct AddToInventory
	{
		AddToInventory(eCubeType type)
			: type(type)
		{}

		const eCubeType type;
	};

	struct SelectedCubeSetPosition
	{
		SelectedCubeSetPosition(const glm::vec3& position)
			: position(position)
		{}

		const glm::vec3 position;
	};

	struct SelectedCubeSetActive
	{
		SelectedCubeSetActive(bool active)
			: active(active)
		{}

		const bool active;
	}; 

	struct AddItemGUI
	{
		AddItemGUI(eCubeType type, eInventoryIndex index)
			: type(type),
			index(index)
		{}

		const eCubeType type;
		const eInventoryIndex index;
	};

	struct RemoveItemGUI
	{
		RemoveItemGUI(eInventoryIndex index)
			: index(index)
		{}

		const eInventoryIndex index;
	};

	struct UpdateSelectionBoxGUI
	{
		UpdateSelectionBoxGUI(eInventoryIndex index)
			: index(index)
		{}

		const eInventoryIndex index;
	};

	struct UpdateItemQuantityGUI
	{
		UpdateItemQuantityGUI(eInventoryIndex index, int quantity)
			:index(index),
			quantity(quantity)
		{}

		const eInventoryIndex index;
		const int quantity;
	};
}