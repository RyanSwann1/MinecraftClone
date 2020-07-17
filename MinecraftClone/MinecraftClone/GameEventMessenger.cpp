#include "GameEventMessenger.h"
#include <assert.h>

namespace
{
	bool isOwnerAlreadyRegistered(const std::unordered_map<eGameEventType, std::vector<Listener>>& listeners, eGameEventType gameEventType, const void* ownerAddress)
	{
		assert(!listeners.empty() || ownerAddress != nullptr);

		auto iter = listeners.find(gameEventType);
		assert(iter != listeners.cend());

		auto result = std::find_if(iter->second.cbegin(), iter->second.cend(), [ownerAddress](const auto& listener) 
		{
			return listener.m_ownerAddress == ownerAddress;
		});

		return result != iter->second.cend();
	}
};

Listener::Listener(const std::function<void(GameEvent)>& fp, const void* ownerAddress)
	: m_listener(fp),
	m_ownerAddress(ownerAddress)
{
	assert(ownerAddress != nullptr);
}

Listener::Listener(Listener&& orig) noexcept
	: m_listener(orig.m_listener),
	m_ownerAddress(orig.m_ownerAddress)
{
	orig.m_listener = nullptr;
	orig.m_ownerAddress = nullptr;
}

Listener& Listener::operator=(Listener&& orig) noexcept
{
	m_listener = orig.m_listener;
	m_ownerAddress = orig.m_ownerAddress;

	orig.m_listener = nullptr;
	orig.m_ownerAddress = nullptr;

	return *this;
}

void GameEventMessenger::subscribe(const std::function<void(GameEvent)>& fp, eGameEventType gameEventType, const void* ownerAddress)
{
	assert(!isOwnerAlreadyRegistered(m_listeners, gameEventType, ownerAddress));
	
	auto iter = m_listeners.find(gameEventType);
	if (iter != m_listeners.cend())
	{
		iter->second.emplace_back(fp, ownerAddress);
	}
	else
	{
		assert(ownerAddress != nullptr);

		m_listeners.emplace(gameEventType, std::vector<Listener>{}).first->second.emplace_back(fp, ownerAddress);
	}
}

void GameEventMessenger::broadcast(GameEvent message, eGameEventType gameEventType)
{
	auto iter = m_listeners.find(gameEventType);
	assert(iter != m_listeners.cend());

	for (const auto& listener : iter->second)
	{
		listener.m_listener(message);
	}
}

void GameEventMessenger::unsubscribe(eGameEventType gameEventType, const void* ownerAddress)
{
	assert(ownerAddress != nullptr &&
		isOwnerAlreadyRegistered(m_listeners, gameEventType, ownerAddress));

	auto iter = m_listeners.find(gameEventType);
	assert(iter != m_listeners.cend());

	auto listener = std::find_if(iter->second.begin(), iter->second.end(), [ownerAddress](const auto& listener)
	{
		return listener.m_ownerAddress == ownerAddress;
	});

	assert(listener != iter->second.end());
	iter->second.erase(listener);
}