#include "SelectedVoxelVisual.h"
#include "MeshGenerator.h"
#include "GameMessenger.h"
#include "GameMessages.h"

SelectedVoxelVisual::SelectedVoxelVisual()
	: m_mesh(),
	m_position(),
	m_active(false)
{
	subscribeToMessenger<GameMessages::SelectedCubeSetActive>(
		[this](const GameMessages::SelectedCubeSetActive& gameMessage) { return onSetActive(gameMessage); }, this);
	
	subscribeToMessenger<GameMessages::SelectedCubeSetPosition>(
		[this](const GameMessages::SelectedCubeSetPosition& gameMessage) { return onSetPosition(gameMessage); }, this);
}

SelectedVoxelVisual::~SelectedVoxelVisual()
{
	unsubscribeToMessenger<GameMessages::SelectedCubeSetActive>(this);
	unsubscribeToMessenger<GameMessages::SelectedCubeSetPosition>(this);
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