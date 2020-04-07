#pragma once

#include "NonCopyable.h"
#include "NonMovable.h"
#include <memory>

class CubeMap : private NonCopyable, private NonMovable
{
public:
	static std::unique_ptr<CubeMap> create();
	~CubeMap();

	void bind() const;
	void unbind() const;

private:
	CubeMap(unsigned int ID);
	unsigned int m_ID;
	unsigned int m_VAOID;
	unsigned int m_VBOID;
};