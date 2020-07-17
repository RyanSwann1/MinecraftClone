#pragma once

#include "NonCopyable.h"
#include "NonMovable.h"
#include <functional>
#include <vector>
#include <assert.h>
#include <array>

enum class eGameEventType
{
	SpawnPickup = 0,
	AddToPlayerInventory,
	Max = AddToPlayerInventory
};

struct Listener : private NonCopyable
{
	Listener(const std::function<void(const void*)>& fp, const void* ownerAddress);
	Listener(Listener&&) noexcept;
	Listener& operator=(Listener&&) noexcept;

	std::function<void(const void*)> m_listener;
	const void* m_ownerAddress;
};

class GameEventMessenger : private NonCopyable, private NonMovable 
{
public:
	static GameEventMessenger& getInstance()
	{
		static GameEventMessenger instance;
		return instance;
	}

	void subscribe(eGameEventType gameEventType, const std::function<void(const void*)>& fp, const void* ownerAddress);
	void unsubscribe(eGameEventType gameEventType, const void* ownerAddress);

	template <typename GameEvent>
	void broadcast(eGameEventType gameEventType, GameEvent gameEvent)
	{
		const auto& listeners = m_listeners[static_cast<int>(gameEventType)];
		for (const auto& listener : listeners)
		{
			listener.m_listener(&gameEvent);
		}
	}

private:
	GameEventMessenger() {}
	std::array<std::vector<Listener>, static_cast<size_t>(eGameEventType::Max) + 1> m_listeners;
};