#include "Timer.h"

Timer::Timer()
	: m_expirationTime(0.0f),
	m_elaspedTime(0.0f),
	m_active(false)
{}

Timer::Timer(float expirationTime, bool active)
	: m_expirationTime(expirationTime),
	m_elaspedTime(0.0f),
	m_active(active)
{}

bool Timer::isActive() const
{
	return m_active;
}

bool Timer::isExpired() const
{
	return m_elaspedTime >= m_expirationTime;
}

float Timer::getElaspedTime() const
{
	return m_elaspedTime;
}

void Timer::update(float deltaTime)
{
	if (m_active)
	{
		m_elaspedTime += deltaTime;

		if (isExpired())
		{
			m_active = false;
		}
	}
}

void Timer::resetElaspedTime()
{
	m_elaspedTime = 0.0f;
}

void Timer::setActive(bool active)
{
	m_active = active;
}

void Timer::setNewExpirationTime(float newExpirationTime)
{
	m_expirationTime = newExpirationTime;
}