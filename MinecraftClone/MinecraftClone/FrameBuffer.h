#pragma once

#include "NonCopyable.h"
#include "NonMovable.h"
#include "glm/glm.hpp"

class FrameBuffer : private NonCopyable, private NonMovable
{
public:
	FrameBuffer(const glm::uvec2& windowSize);
	~FrameBuffer();

	unsigned int getID() const;
	unsigned int getVAOID() const;
	unsigned int getTextureID() const;
	unsigned int getRenderBufferID() const;

	//void bind() const;
	//void unbind() const;
	//void render() const;

private:
	unsigned int m_ID;
	unsigned int m_vaoID;
	unsigned int m_positionsVBO;
	unsigned int m_textCoordsVBO;
	unsigned int m_textureID;
	unsigned int m_renderBufferID;
};