#include "SelectedVoxelVisual.h"
#include "MeshGenerator.h"
#include "GameEventMessenger.h"
#include "GameEvents.h"

SelectedVoxelVisual::SelectedVoxelVisual()
	: m_mesh(),
	m_position(),
	m_active(false)
{
	GameEventMessenger::getInstance().subscribe<GameEvents::SelectedCubeSetActive>(std::bind(
		&SelectedVoxelVisual::onSetActive, this, std::placeholders::_1), this);

	GameEventMessenger::getInstance().subscribe<GameEvents::SelectedCubeSetPosition>(std::bind(
		&SelectedVoxelVisual::onSetPosition, this, std::placeholders::_1), this);
}

SelectedVoxelVisual::~SelectedVoxelVisual()
{
	GameEventMessenger::getInstance().unsubscribe<GameEvents::SelectedCubeSetActive>(this);
	GameEventMessenger::getInstance().unsubscribe<GameEvents::SelectedCubeSetPosition>(this);
}

void SelectedVoxelVisual::render()
{
	if (m_active)
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

void SelectedVoxelVisual::onSetPosition(const GameEvents::SelectedCubeSetPosition& gameEvent)
{
	if (m_position != gameEvent.position || !m_active)
	{
		m_position = gameEvent.position;
		m_mesh.m_transparentVertexBuffer.clear();
		MeshGenerator::generateVoxelSelectionMesh(m_mesh.m_transparentVertexBuffer, m_position);

		m_active = true;
	}
}

void SelectedVoxelVisual::onSetActive(const GameEvents::SelectedCubeSetActive& gameEvent)
{
	m_active = gameEvent.active;
}