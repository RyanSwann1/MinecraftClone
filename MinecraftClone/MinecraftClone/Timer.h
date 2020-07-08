#pragma once

class Timer
{
public:
	Timer();
	Timer(float expirationTime, bool active = true);

	bool isActive() const;
	bool isExpired() const;
	float getElaspedTime() const;
	float getExpirationTime() const;

	void update(float deltaTime);
	void resetElaspedTime();
	void setActive(bool active);
	void setNewExpirationTime(float newExpirationTime);

private:
	float m_expirationTime;
	float m_elaspedTime;
	bool m_active;
};