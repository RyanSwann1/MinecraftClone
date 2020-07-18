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

	struct PlayerDisgardPickup
	{
		PlayerDisgardPickup(eCubeType cubeType, const glm::vec3& position, const glm::vec3& initialVelocity)
			: cubeType(cubeType),
			position(position),
			initialVelocity(initialVelocity)
		{}
		
		eCubeType cubeType;
		glm::vec3 position;
		glm::vec3 initialVelocity;
	};

	struct AddToInventory
	{
		AddToInventory(eCubeType type)
			: type(type)
		{}

		eCubeType type;
	};

	struct SelectedCubeSetPosition
	{
		SelectedCubeSetPosition(const glm::vec3& position)
			: position(position)
		{}

		glm::vec3 position;
	};

	struct SelectedCubeSetActive
	{
		SelectedCubeSetActive(bool active)
			: active(active)
		{}

		bool active;
	};

	struct AddItemGUI
	{
		AddItemGUI(eCubeType type, eInventoryIndex index)
			: type(type),
			index(index)
		{}

		eCubeType type;
		eInventoryIndex index;
	};

	struct RemoveItemGUI
	{
		RemoveItemGUI(eInventoryIndex index)
			: index(index)
		{}

		eInventoryIndex index;
	};

	struct UpdateSelectionBoxGUI
	{
		UpdateSelectionBoxGUI(eInventoryIndex index)
			: index(index)
		{}

		eInventoryIndex index;
	};

	struct UpdateItemQuantityGUI
	{
		UpdateItemQuantityGUI(eInventoryIndex index, int quantity)
			:index(index),
			quantity(quantity)
		{}

		eInventoryIndex index;
		int quantity;
	};
}