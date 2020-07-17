#include "GameEventMessenger.h"

namespace
{
	bool isOwnerAlreadyRegistered(const std::vector<Listener>& listeners, eGameEventType gameEventType, const void* ownerAddress)
	{
		assert(ownerAddress != nullptr);

		if (!listeners.empty())
		{
			auto result = std::find_if(listeners.cbegin(), listeners.cend(), [ownerAddress](const auto& listener)
			{
				return listener.m_ownerAddress == ownerAddress;
			});

			return result != listeners.cend();
		}
		else
		{
			return false;
		}
	}
};

Listener::Listener(const std::function<void(const void*)>& fp, const void* ownerAddress)
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

void GameEventMessenger::subscribe(eGameEventType gameEventType, const std::function<void(const void*)>& fp, const void* ownerAddress)
{
	auto& listeners = m_listeners[static_cast<int>(gameEventType)];
	assert(!isOwnerAlreadyRegistered(listeners, gameEventType, ownerAddress));
	
	listeners.emplace_back(fp, ownerAddress);
}

void GameEventMessenger::unsubscribe(eGameEventType gameEventType, const void* ownerAddress)
{
	auto& listeners = m_listeners[static_cast<int>(gameEventType)];
	assert(ownerAddress != nullptr &&
		isOwnerAlreadyRegistered(listeners, gameEventType, ownerAddress));

	auto iter = std::find_if(listeners.begin(), listeners.end(), [ownerAddress](const auto& listener)
	{
		return listener.m_ownerAddress == ownerAddress;
	});

	assert(iter != listeners.end());
	listeners.erase(iter);
}