#include "Frustum.h"
#include "Utilities.h"

//Plane
Frustum::Plane::Plane()
	: d(0.0f),
	n()
{}

//Frustum
Frustum::Frustum()
	: m_planes()
{}

void Frustum::update(const glm::mat4& mat)
{
	m_planes[ePlaneSide::Right].n.x = mat[0][3] - mat[0][0];
	m_planes[ePlaneSide::Right].n.y = mat[1][3] - mat[1][0];
	m_planes[ePlaneSide::Right].n.z = mat[2][3] - mat[2][0];
	m_planes[ePlaneSide::Right].d = mat[3][3] - mat[3][0];

	m_planes[ePlaneSide::Left].n.x = mat[0][3] + mat[0][0];
	m_planes[ePlaneSide::Left].n.y = mat[1][3] + mat[1][0];
	m_planes[ePlaneSide::Left].n.z = mat[2][3] + mat[2][0];
	m_planes[ePlaneSide::Left].d = mat[3][3] + mat[3][0];

	m_planes[ePlaneSide::Bottom].n.x = mat[0][3] + mat[0][1];
	m_planes[ePlaneSide::Bottom].n.y = mat[1][3] + mat[1][1];
	m_planes[ePlaneSide::Bottom].n.z = mat[2][3] + mat[2][1];
	m_planes[ePlaneSide::Bottom].d = mat[3][3] + mat[3][1];

	m_planes[ePlaneSide::Top].n.x = mat[0][3] - mat[0][1];
	m_planes[ePlaneSide::Top].n.y = mat[1][3] - mat[1][1];
	m_planes[ePlaneSide::Top].n.z = mat[2][3] - mat[2][1];
	m_planes[ePlaneSide::Top].d = mat[3][3] - mat[3][1];

	m_planes[ePlaneSide::Far].n.x = mat[0][3] - mat[0][2];
	m_planes[ePlaneSide::Far].n.y = mat[1][3] - mat[1][2];
	m_planes[ePlaneSide::Far].n.z = mat[2][3] - mat[2][2];
	m_planes[ePlaneSide::Far].d = mat[3][3] - mat[3][2];

	m_planes[ePlaneSide::Near].n.x = mat[0][3] + mat[0][2];
	m_planes[ePlaneSide::Near].n.y = mat[1][3] + mat[1][2];
	m_planes[ePlaneSide::Near].n.z = mat[2][3] + mat[2][2];
	m_planes[ePlaneSide::Near].d = mat[3][3] + mat[3][2];
}

bool Frustum::isChunkInFustrum(const glm::vec3& chunkStartingPosition) const
{
	glm::ivec3 chunkCentrePosition;
	chunkCentrePosition.x = chunkStartingPosition.x + Utilities::CHUNK_WIDTH / 2;
	chunkCentrePosition.y = chunkStartingPosition.y + Utilities::CHUNK_HEIGHT / 2;
	chunkCentrePosition.z = chunkStartingPosition.z + Utilities::CHUNK_DEPTH / 2;

	for (const auto& plane : m_planes)
	{
		//Back
		if (glm::dot(glm::vec3(chunkCentrePosition.x - Utilities::CHUNK_WIDTH / 2,
			chunkCentrePosition.y - Utilities::CHUNK_HEIGHT / 2,
			chunkCentrePosition.z - Utilities::CHUNK_DEPTH / 2), plane.n) + plane.d >= 0)
		{
			return true;
		}

		if (glm::dot(glm::vec3(chunkCentrePosition.x + Utilities::CHUNK_WIDTH / 2,
			chunkCentrePosition.y - Utilities::CHUNK_HEIGHT / 2,
			chunkCentrePosition.z - Utilities::CHUNK_DEPTH / 2), plane.n) + plane.d >= 0)
		{

			return true;
		}

		if (glm::dot(glm::vec3(chunkCentrePosition.x - Utilities::CHUNK_WIDTH / 2,
			chunkCentrePosition.y + Utilities::CHUNK_HEIGHT / 2,
			chunkCentrePosition.z - Utilities::CHUNK_DEPTH / 2), plane.n) + plane.d >= 0)
		{
			return true;
		}

		if (glm::dot(glm::vec3(chunkCentrePosition.x + Utilities::CHUNK_WIDTH / 2,
			chunkCentrePosition.y + Utilities::CHUNK_HEIGHT / 2,
			chunkCentrePosition.z - Utilities::CHUNK_DEPTH / 2), plane.n) + plane.d >= 0)
		{
			return true;
		}

		//Front
		if (glm::dot(glm::vec3(chunkCentrePosition.x - Utilities::CHUNK_WIDTH / 2,
			chunkCentrePosition.y - Utilities::CHUNK_HEIGHT / 2,
			chunkCentrePosition.z + Utilities::CHUNK_DEPTH / 2), plane.n) + plane.d >= 0)
		{

			return true;
		}


		if (glm::dot(glm::vec3(chunkCentrePosition.x + Utilities::CHUNK_WIDTH / 2,
			chunkCentrePosition.y - Utilities::CHUNK_HEIGHT / 2,
			chunkCentrePosition.z + Utilities::CHUNK_DEPTH / 2), plane.n) + plane.d >= 0)
		{
			return true;
		}

		if (glm::dot(glm::vec3(chunkCentrePosition.x - Utilities::CHUNK_WIDTH / 2,
			chunkCentrePosition.y + Utilities::CHUNK_HEIGHT / 2,
			chunkCentrePosition.z + Utilities::CHUNK_DEPTH / 2), plane.n) + plane.d >= 0)
		{
			return true;
		}

		if (glm::dot(glm::vec3(chunkCentrePosition.x + Utilities::CHUNK_WIDTH / 2,
			chunkCentrePosition.y + Utilities::CHUNK_HEIGHT / 2,
			chunkCentrePosition.z + Utilities::CHUNK_DEPTH / 2), plane.n) + plane.d >= 0)
		{
			return true;
		}

		return false;
	}
}