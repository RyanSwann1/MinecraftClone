#pragma once

#include "NonCopyable.h"
#include "NonMovable.h"

class Gui : private NonCopyable, private NonMovable
{
public:
	Gui();
	~Gui();

	void render() const;

private:
	unsigned int m_ID;
};