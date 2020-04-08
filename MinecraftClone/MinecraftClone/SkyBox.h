#pragma once

#include "NonCopyable.h"
#include "NonMovable.h"
#include <memory>

class SkyBox : private NonCopyable, private NonMovable
{
public:
	static std::unique_ptr<SkyBox> create();
	~SkyBox();

	void render() const;

private:
	SkyBox(unsigned int cubeMapID);

	unsigned int m_cubeMapID;
	unsigned int m_vaoID;
	unsigned int m_vboID;
};