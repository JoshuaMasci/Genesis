#include "Frustum.hpp"

using namespace Genesis;

Frustum::Frustum(const matrix4F& view_projection)
{
	matrix4F m = glm::transpose(view_projection);

	planes[Plane::Left] = m[3] + m[0];
	planes[Plane::Right] = m[3] - m[0];
	planes[Plane::Bottom] = m[3] + m[1];
	planes[Plane::Top] = m[3] - m[1];
	planes[Plane::Near] = m[3];
	planes[Plane::Far] = m[3] - m[2];

	for (uint8_t i = 0; i < Plane::Count; i++)
	{
		planes[i] = glm::normalize(planes[i]);
	}
}

bool Frustum::sphereTest(vector3F& postion, float radius)
{
	for (uint8_t i = 0; i < Plane::Count; i++)
	{
		vector3F plane_normal = vector3F(planes[i].x, planes[i].y, planes[i].z);
		float plane_distance = planes[i].w;

		if ((glm::dot(postion, plane_normal) + plane_distance + radius) <= 0)
		{
			return false;
		}
	}

	return true;
}
