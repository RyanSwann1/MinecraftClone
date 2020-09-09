#include "SelectedVoxelVisual.h"
#include "MeshGenerator.h"
#include "GameMessenger.h"
#include "GameMessages.h"

SelectedVoxelVisual::SelectedVoxelVisual()
	: m_mesh(),
	m_position(),
	m_active(false)
{
	GameMessenger::getInstance().subscribe<GameMessages::SelectedCubeSetActive>(std::bind(
		&SelectedVoxelVisual::onSetActive, this, std::placeholders::_1), this);

	GameMessenger::getInstance().subscribe<GameMessages::SelectedCubeSetPosition>(std::bind(
		&SelectedVoxelVisual::onSetPosition, this, std::placeholders::_1), this);
}

SelectedVoxelVisual::~SelectedVoxelVisual()
{
	GameMessenger::getInstance().unsubscribe<GameMessages::SelectedCubeSetActive>(this);
	GameMessenger::getInstance().unsubscribe<GameMessages::SelectedCubeSetPosition>(this);
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

void SelectedVoxelVisual::onSetPosition(const GameMessages::SelectedCubeSetPosition& gameMessage)
{
	if (m_position != gameMessage.position || !m_active)
	{
		m_position = gameMessage.position;
		m_mesh.m_transparentVertexBuffer.clear();
		MeshGenerator::generateVoxelSelectionMesh(m_mesh.m_transparentVertexBuffer, m_position);

		m_active = true;
	}
}

void SelectedVoxelVisual::onSetActive(const GameMessages::SelectedCubeSetActive& gameMessage)
{
	m_active = gameMessage.active;
}