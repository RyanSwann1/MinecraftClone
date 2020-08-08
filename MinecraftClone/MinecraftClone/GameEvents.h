#pragma once

#include "Globals.h"

enum class eGameEventType;
namespace GameEvents
{
	//CRTP - C++
	template <eGameEventType T>
	struct BaseEvent
	{		
		static eGameEventType getType() { return T; };
	};

	struct SpawnPickUp : public BaseEvent<eGameEventType::SpawnPickup>
	{
		SpawnPickUp(eCubeType type, const glm::vec3& position)
			: type(type),
			position(position)
		{}

		const eCubeType type;
		const glm::vec3 position;
	};

	struct PlayerDisgardPickup : public BaseEvent<eGameEventType::PlayerDisgardPickup>
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

	struct AddToInventory : public BaseEvent<eGameEventType::AddToPlayerInventory>
	{
		AddToInventory(eCubeType type)
			: type(type)
		{}

		const eCubeType type;
	};

	struct DestroyCubeReset : public BaseEvent<eGameEventType::DestroyCubeReset>
	{};

	struct DestroyCubeSetPosition : public BaseEvent<eGameEventType::DestroyCubeSetPosition>
	{
		DestroyCubeSetPosition(const glm::vec3& position, float destroyCubeTimerExpire)
			: position(position),
			destroyCubeTimerExpire(destroyCubeTimerExpire)
		{}

		const glm::vec3 position;
		const float destroyCubeTimerExpire;
	};

	struct SelectedCubeSetPosition : public BaseEvent<eGameEventType::SelectedCubeSetPosition>
	{
		SelectedCubeSetPosition(const glm::vec3& position)
			: position(position)
		{}

		const glm::vec3 position;
	};

	struct SelectedCubeSetActive : public BaseEvent<eGameEventType::SelectedCubeSetActive>
	{
		SelectedCubeSetActive(bool active)
			: active(active)
		{}

		const bool active;
	}; 

	struct AddItemGUI : public BaseEvent<eGameEventType::AddItemGUI>
	{
		AddItemGUI(eCubeType type, eInventoryIndex index)
			: type(type),
			index(index)
		{}

		const eCubeType type;
		const eInventoryIndex index;
	};

	struct RemoveItemGUI : public BaseEvent<eGameEventType::RemoveItemGUI>
	{
		RemoveItemGUI(eInventoryIndex index)
			: index(index)
		{}

		const eInventoryIndex index;
	};

	struct UpdateSelectionBoxGUI : public BaseEvent<eGameEventType::UpdateSelectionBoxGUI>
	{
		UpdateSelectionBoxGUI(eInventoryIndex index)
			: index(index)
		{}

		const eInventoryIndex index;
	};

	struct UpdateItemQuantityGUI : public BaseEvent<eGameEventType::UpdateItemQuantityGUI>
	{
		UpdateItemQuantityGUI(eInventoryIndex index, int quantity)
			:index(index),
			quantity(quantity)
		{}

		const eInventoryIndex index;
		const int quantity;
	};
}