#pragma once

#include "Globals.h"

enum class eGameMessageType;
namespace GameMessages
{
	//CRTP - C++
	template <eGameMessageType T>
	struct BaseMessage
	{		
		static eGameMessageType getType() { return T; };
	};

	struct SpawnPickUp : public BaseMessage<eGameMessageType::SpawnPickup>
	{
		SpawnPickUp(eCubeType type, const glm::vec3& position)
			: type(type),
			position(position)
		{}

		const eCubeType type;
		const glm::vec3 position;
	};

	struct PlayerDisgardPickup : public BaseMessage<eGameMessageType::PlayerDisgardPickup>
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

	struct AddToInventory : public BaseMessage<eGameMessageType::AddToPlayerInventory>
	{
		AddToInventory(eCubeType type)
			: type(type)
		{}

		const eCubeType type;
	};

	struct DestroyCubeReset : public BaseMessage<eGameMessageType::DestroyCubeReset>
	{};

	struct DestroyCubeSetPosition : public BaseMessage<eGameMessageType::DestroyCubeSetPosition>
	{
		DestroyCubeSetPosition(const glm::vec3& position, float destroyCubeTimerExpire)
			: position(position),
			destroyCubeTimerExpire(destroyCubeTimerExpire)
		{}

		const glm::vec3 position;
		const float destroyCubeTimerExpire;
	};

	struct SelectedCubeSetPosition : public BaseMessage<eGameMessageType::SelectedCubeSetPosition>
	{
		SelectedCubeSetPosition(const glm::vec3& position)
			: position(position)
		{}

		const glm::vec3 position;
	};

	struct SelectedCubeSetActive : public BaseMessage<eGameMessageType::SelectedCubeSetActive>
	{
		SelectedCubeSetActive(bool active)
			: active(active)
		{}

		const bool active;
	}; 

	struct AddItemGUI : public BaseMessage<eGameMessageType::AddItemGUI>
	{
		AddItemGUI(eCubeType type, eInventoryIndex index)
			: type(type),
			index(index)
		{}

		const eCubeType type;
		const eInventoryIndex index;
	};

	struct RemoveItemGUI : public BaseMessage<eGameMessageType::RemoveItemGUI>
	{
		RemoveItemGUI(eInventoryIndex index)
			: index(index)
		{}

		const eInventoryIndex index;
	};

	struct UpdateSelectionBoxGUI : public BaseMessage<eGameMessageType::UpdateSelectionBoxGUI>
	{
		UpdateSelectionBoxGUI(eInventoryIndex index)
			: index(index)
		{}

		const eInventoryIndex index;
	};

	struct UpdateItemQuantityGUI : public BaseMessage<eGameMessageType::UpdateItemQuantityGUI>
	{
		UpdateItemQuantityGUI(eInventoryIndex index, int quantity)
			:index(index),
			quantity(quantity)
		{}

		const eInventoryIndex index;
		const int quantity;
	};
}