#include "FrameBuffer.h"
#include "Globals.h"
#include "glad.h"

namespace
{
	constexpr std::array<glm::vec2, 6> QUAD
	{
		glm::vec2(-1.0f, -1.0f),
		glm::vec2(1.0f, -1.0f),
		glm::vec2(1.0f, 1.0f),
		glm::vec2(1.0f, 1.0f),
		glm::vec2(-1.0f, 1.0f),
		glm::vec2(-1.0f, -1.0f)
	};

	constexpr std::array<glm::vec2, 6> TEXT_COORDS
	{
		glm::vec2(0.0f, 0.0f),
		glm::vec2(1.0f, 0.0f),
		glm::vec2(1.0f, 1.0f),
		glm::vec2(1.0f, 1.0f),
		glm::vec2(0.0f, 1.0f),
		glm::vec2(0.0f, 0.0f)
	};
}

FrameBuffer::FrameBuffer(const glm::uvec2& windowSize)
	: m_ID(Globals::INVALID_OPENGL_ID),
	m_vaoID(Globals::INVALID_OPENGL_ID),
	m_positionsVBO(Globals::INVALID_OPENGL_ID),
	m_textCoordsVBO(Globals::INVALID_OPENGL_ID),
	m_textureID(Globals::INVALID_OPENGL_ID),
	m_renderBufferID(Globals::INVALID_OPENGL_ID)
{
	glGenFramebuffers(1, &m_ID);
	glBindFramebuffer(GL_FRAMEBUFFER, m_ID);

	glGenTextures(1, &m_textureID);
	glBindTexture(GL_TEXTURE_2D, m_textureID);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, windowSize.x, windowSize.y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_textureID, 0);
	
	glGenRenderbuffers(1, &m_renderBufferID);
	glBindRenderbuffer(GL_RENDERBUFFER, m_renderBufferID);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, windowSize.x, windowSize.y); // use a single renderbuffer object for both a depth AND stencil buffer.
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_renderBufferID); // now actually attach it
	
	// now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
	assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
	

	glGenVertexArrays(1, &m_vaoID);
	glBindVertexArray(m_vaoID);
	
	glGenBuffers(1, &m_positionsVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_positionsVBO);
	glBufferData(GL_ARRAY_BUFFER, QUAD.size() * sizeof(glm::vec2), QUAD.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (const void*)0);

	glGenBuffers(1, &m_textCoordsVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_textCoordsVBO);
	glBufferData(GL_ARRAY_BUFFER, TEXT_COORDS.size() * sizeof(glm::vec2), TEXT_COORDS.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (const void*)0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

FrameBuffer::~FrameBuffer()
{
	assert(m_ID != Globals::INVALID_OPENGL_ID);
	glDeleteFramebuffers(1, &m_ID);

	assert(m_vaoID != Globals::INVALID_OPENGL_ID);
	glDeleteVertexArrays(1, &m_vaoID);

	assert(m_textureID != Globals::INVALID_OPENGL_ID);
	glDeleteTextures(1, &m_textureID);

	assert(m_renderBufferID != Globals::INVALID_OPENGL_ID);
	glDeleteRenderbuffers(1, &m_renderBufferID);

	assert(m_positionsVBO != Globals::INVALID_OPENGL_ID);
	glDeleteBuffers(1, &m_positionsVBO);

	assert(m_textCoordsVBO != Globals::INVALID_OPENGL_ID);
	glDeleteBuffers(1, &m_textCoordsVBO);
}

unsigned int FrameBuffer::getID() const
{
	return m_ID;
}

unsigned int FrameBuffer::getVAOID() const
{
	return m_vaoID;
}

unsigned int FrameBuffer::getTextureID() const
{
	return m_textureID;
}

unsigned int FrameBuffer::getRenderBufferID() const
{
	return m_renderBufferID;
}
//
//void FrameBuffer::bind() const
//{
//
//}
//
//void FrameBuffer::unbind() const
//{
//}
//
//void FrameBuffer::render() const
//{
//}
