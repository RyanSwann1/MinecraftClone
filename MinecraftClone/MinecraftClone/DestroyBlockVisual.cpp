#include "DestroyBlockVisual.h"
#include "MeshGenerator.h"
#include <assert.h>
#include <iostream>
#include <SFML/Graphics.hpp>

namespace
{
	constexpr float EXPIRATION_TIME = 0.05f;
}

DestroyBlockVisual::DestroyBlockVisual()
	: m_mesh(),
	m_timer(),
	m_index(eDestroyCubeIndex::One),
	m_currentCubePosition()
{
	m_timer.setNewExpirationTime(EXPIRATION_TIME);
}

bool DestroyBlockVisual::isCompleted() const
{
	return m_index == eDestroyCubeIndex::Max;
}

void DestroyBlockVisual::setPosition(const glm::ivec3& position)
{
	if (m_currentCubePosition != position || !m_timer.isActive())
	{
		m_index = eDestroyCubeIndex::One;
		m_timer.resetElaspedTime();
		m_timer.setActive(true);
		m_currentCubePosition = position;
		m_mesh.m_transparentVertexBuffer.clear();
		MeshGenerator::generateDestroyBlockMesh(m_mesh.m_transparentVertexBuffer, m_index, m_currentCubePosition);
	}
}

void DestroyBlockVisual::reset()
{
	m_index = eDestroyCubeIndex::One;
	m_timer.resetElaspedTime();
	m_timer.setActive(false);
	m_mesh.m_transparentVertexBuffer.clear();
}

void DestroyBlockVisual::update(float deltaTime)
{
	m_timer.update(deltaTime);
	if (m_timer.isExpired())
	{
		if (static_cast<int>(m_index) < static_cast<int>(eDestroyCubeIndex::Max))
		{
			m_timer.setActive(true);
			m_index = static_cast<eDestroyCubeIndex>(static_cast<int>(m_index) + 1); 
			m_mesh.m_transparentVertexBuffer.clear();
			MeshGenerator::generateDestroyBlockMesh(m_mesh.m_transparentVertexBuffer, m_index, m_currentCubePosition);
		
			if (m_index == eDestroyCubeIndex::Max)
			{
				m_timer.setActive(false);
			}
			else
			{
				m_timer.setActive(true);
			}

			m_timer.resetElaspedTime();
		}
	}
}

void DestroyBlockVisual::render()
{
	if (m_mesh.m_transparentVertexBuffer.bindToVAO)
	{
		m_mesh.attachTransparentVBO();
	}

	if (m_mesh.m_transparentVertexBuffer.displayable)
	{
		m_mesh.bindTransparentVAO();
		glDrawElements(GL_TRIANGLES, m_mesh.m_transparentVertexBuffer.indicies.size(), GL_UNSIGNED_INT, nullptr);
	}
}