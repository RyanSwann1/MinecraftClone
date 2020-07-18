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

	struct AddToInventory
	{
		AddToInventory(eCubeType type)
			: type(type)
		{}

		eCubeType type;
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