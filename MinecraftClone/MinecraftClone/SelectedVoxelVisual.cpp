#include "SelectedVoxelVisual.h"
#include "MeshGenerator.h"
#include "GameMessenger.h"
#include "GameMessages.h"

SelectedVoxelVisual::SelectedVoxelVisual()
	: m_mesh(),
	m_position(),
	m_active(false)
{}

void SelectedVoxelVisual::setPosition(const glm::vec3& position)
{
	if (m_position != position || !m_active)
	{
		m_position = position;
		m_mesh.m_transparentVertexBuffer.clear();
		MeshGenerator::generateVoxelSelectionMesh(m_mesh.m_transparentVertexBuffer, m_position);

		m_active = true;
	}
}

void SelectedVoxelVisual::setActive(bool active)
{
	m_active = active;
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