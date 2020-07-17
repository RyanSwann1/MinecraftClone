#pragma once

#include "NonCopyable.h"
#include "NonMovable.h"
#include <unordered_map>
#include <functional>
#include <vector>

enum class eGameEventType
{
	NeedMoarEvents = 0
};

struct GameEvent
{
	GameEvent(const void* data = nullptr)
		: data(data)
	{}

	const void* data;
};

struct Listener : private NonCopyable
{
	Listener(const std::function<void(GameEvent)>& fp, const void* ownerAddress);
	Listener(Listener&&) noexcept;
	Listener& operator=(Listener&&) noexcept;

	std::function<void(GameEvent)> m_listener;
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

	void subscribe(const std::function<void(GameEvent)>& fp, eGameEventType message, const void* ownerAddress);
	void broadcast(GameEvent message, eGameEventType gameEventType);
	void unsubscribe(eGameEventType message, const void* ownerAddress);

private:
	GameEventMessenger();
	std::unordered_map<eGameEventType, std::vector<Listener>> m_listeners;
};