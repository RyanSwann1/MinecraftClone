#include "SelectedVoxelVisual.h"
#include "MeshGenerator.h"
#include "GameEventMessenger.h"
#include "GameEvents.h"

SelectedVoxelVisual::SelectedVoxelVisual()
	: m_mesh(),
	m_position(),
	m_active(false)
{
	GameEventMessenger::getInstance().subscribe(eGameEventType::SelectedCubeSetActive, std::bind(
		&SelectedVoxelVisual::onSetActive, this, std::placeholders::_1), this);

	GameEventMessenger::getInstance().subscribe(eGameEventType::SelectedCubeSetPosition, std::bind(
		&SelectedVoxelVisual::onSetPosition, this, std::placeholders::_1), this);
}

SelectedVoxelVisual::~SelectedVoxelVisual()
{
	GameEventMessenger::getInstance().unsubscribe(eGameEventType::SelectedCubeSetActive, this);
	GameEventMessenger::getInstance().unsubscribe(eGameEventType::SelectedCubeSetPosition, this);
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

void SelectedVoxelVisual::onSetPosition(const void* gameEvent)
{
	assert(gameEvent != nullptr);
	const auto* setPositionEvent = static_cast<const GameEvents::SelectedCubeSetPosition*>(gameEvent);

	if (m_position != setPositionEvent->position || !m_active)
	{
		m_position = setPositionEvent->position;
		m_mesh.m_transparentVertexBuffer.clear();
		MeshGenerator::generateVoxelSelectionMesh(m_mesh.m_transparentVertexBuffer, m_position);

		m_active = true;
	}
}

void SelectedVoxelVisual::onSetActive(const void* gameEvent)
{
	assert(gameEvent != nullptr);
	const auto* setActiveEvent = static_cast<const GameEvents::SelectedCubeSetActive*>(gameEvent);

	m_active = setActiveEvent->active;
}
