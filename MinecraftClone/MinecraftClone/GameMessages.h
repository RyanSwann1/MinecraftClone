#pragma once

#include "Globals.h"

//https://stackoverflow.com/questions/4173254/what-is-the-curiously-recurring-template-pattern-crtp
namespace GameMessages
{
	struct SpawnPickUp 
	{
		const eCubeType type;
		const glm::vec3 position;
	};

	struct PlayerDisgardPickup
	{
		const eCubeType cubeType;
		const glm::vec3 position;
		const glm::vec3 initialVelocity;
	};

	struct AddToInventory { const eCubeType type; };

	struct SelectedCubeSetPosition { const glm::vec3 position; };

	struct SelectedCubeSetActive { const bool active; }; 

	struct AddItemGUI
	{
		const eCubeType type;
		const eInventoryIndex index;
	};

	struct RemoveItemGUI { const eInventoryIndex index; };

	struct UpdateSelectionBoxGUI { const eInventoryIndex index; };

	struct UpdateItemQuantityGUI
	{
		const eInventoryIndex index;
		const int quantity;
	};
}