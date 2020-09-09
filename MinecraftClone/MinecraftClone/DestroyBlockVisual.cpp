#include "DestroyBlockVisual.h"
#include "MeshGenerator.h"
#include "GameMessenger.h"
#include "GameMessages.h"
#include <assert.h>
#include <SFML/Graphics.hpp>

DestroyBlockVisual::DestroyBlockVisual()
	: m_mesh(),
	m_timer(),
	m_index(eDestroyCubeIndex::One),
	m_currentCubePosition()
{
	GameMessenger::getInstance().subscribe<GameMessages::DestroyCubeSetPosition>(
		std::bind(&DestroyBlockVisual::onSetPosition, this, std::placeholders::_1), this);

	GameMessenger::getInstance().subscribe<GameMessages::DestroyCubeReset>(
		std::bind(&DestroyBlockVisual::onReset , this, std::placeholders::_1), this);
}

DestroyBlockVisual::~DestroyBlockVisual()
{
	GameMessenger::getInstance().unsubscribe<GameMessages::DestroyCubeSetPosition>(this);
	GameMessenger::getInstance().unsubscribe<GameMessages::DestroyCubeReset>(this);
}

void DestroyBlockVisual::update(float deltaTime)
{
	m_timer.update(deltaTime);
	if (m_timer.isExpired())
	{
		if (static_cast<int>(m_index) < static_cast<int>(eDestroyCubeIndex::Max))
		{
			m_index = static_cast<eDestroyCubeIndex>(static_cast<int>(m_index) + 1); 
			m_mesh.m_transparentVertexBuffer.clear();
			MeshGenerator::generateDestroyBlockMesh(m_mesh.m_transparentVertexBuffer, m_index, m_currentCubePosition);
		
			if (m_index != eDestroyCubeIndex::Max)
			{
				m_timer.resetElaspedTime();
			}
		}
	}
}

void DestroyBlockVisual::render()
{
	if (m_timer.isActive())
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
}

void DestroyBlockVisual::onSetPosition(const GameMessages::DestroyCubeSetPosition& gameEvent)
{
	reset();
	m_timer.setActive(true);
	m_timer.setNewExpirationTime(gameEvent.destroyCubeTimerExpire / ((static_cast<float>(eDestroyCubeIndex::Max) + 1.0f)));
	m_currentCubePosition = gameEvent.position;

	MeshGenerator::generateDestroyBlockMesh(m_mesh.m_transparentVertexBuffer, m_index, m_currentCubePosition);
}

void DestroyBlockVisual::onReset(const GameMessages::DestroyCubeReset& gameEvent)
{
	reset();
}

void DestroyBlockVisual::reset()
{
	m_index = eDestroyCubeIndex::One;
	m_timer.resetElaspedTime();
	m_timer.setActive(false);
	m_mesh.m_transparentVertexBuffer.clear();
}
