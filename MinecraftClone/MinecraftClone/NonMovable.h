#pragma once

class NonMovable
{
public:
	NonMovable() {}
	NonMovable(NonMovable&&) = delete;
	NonMovable& operator=(NonMovable&&) = delete;
};