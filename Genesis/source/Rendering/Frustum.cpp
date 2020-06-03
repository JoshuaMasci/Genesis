#include "Genesis/Rendering/Frustum.hpp"

namespace Genesis
{
	Frustum::Frustum(const matrix4F& matrix)
	{
		planes[Plane::Left].x = matrix[0].w + matrix[0].x;
		planes[Plane::Left].y = matrix[1].w + matrix[1].x;
		planes[Plane::Left].z = matrix[2].w + matrix[2].x;
		planes[Plane::Left].w = matrix[3].w + matrix[3].x;

		planes[Plane::Right].x = matrix[0].w - matrix[0].x;
		planes[Plane::Right].y = matrix[1].w - matrix[1].x;
		planes[Plane::Right].z = matrix[2].w - matrix[2].x;
		planes[Plane::Right].w = matrix[3].w - matrix[3].x;

		planes[Plane::Top].x = matrix[0].w - matrix[0].y;
		planes[Plane::Top].y = matrix[1].w - matrix[1].y;
		planes[Plane::Top].z = matrix[2].w - matrix[2].y;
		planes[Plane::Top].w = matrix[3].w - matrix[3].y;

		planes[Plane::Bottom].x = matrix[0].w + matrix[0].y;
		planes[Plane::Bottom].y = matrix[1].w + matrix[1].y;
		planes[Plane::Bottom].z = matrix[2].w + matrix[2].y;
		planes[Plane::Bottom].w = matrix[3].w + matrix[3].y;

		planes[Plane::Far].x = matrix[0].w + matrix[0].z;
		planes[Plane::Far].y = matrix[1].w + matrix[1].z;
		planes[Plane::Far].z = matrix[2].w + matrix[2].z;
		planes[Plane::Far].w = matrix[3].w + matrix[3].z;

		planes[Plane::Near].x = matrix[0].w - matrix[0].z;
		planes[Plane::Near].y = matrix[1].w - matrix[1].z;
		planes[Plane::Near].z = matrix[2].w - matrix[2].z;
		planes[Plane::Near].w = matrix[3].w - matrix[3].z;

		for (auto i = 0; i < Plane::Count; i++)
		{
			float length = sqrtf(planes[i].x * planes[i].x + planes[i].y * planes[i].y + planes[i].z * planes[i].z);
			planes[i] /= length;
		}
	}

	bool Frustum::sphereTest(vector3F& pos, float radius)
	{
		for (uint8_t i = 0; i < Plane::Count; i++)
		{
			if ((planes[i].x * pos.x) + (planes[i].y * pos.y) + (planes[i].z * pos.z) + planes[i].w <= -radius)
			{
				return false;
			}
		}

		return true;
	}
}