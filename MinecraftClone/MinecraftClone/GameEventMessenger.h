#pragma once

#include "NonCopyable.h"
#include "NonMovable.h"
#include <unordered_map>
#include <functional>
#include <vector>
#include <assert.h>

enum class eGameEventType
{
	SpawnPickup = 0
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
		auto iter = m_listeners.find(gameEventType);
		assert(iter != m_listeners.cend());

		for (const auto& listener : iter->second)
		{
			listener.m_listener(&gameEvent);
		}
	}

private:
	GameEventMessenger() {}
	std::unordered_map<eGameEventType, std::vector<Listener>> m_listeners;
};