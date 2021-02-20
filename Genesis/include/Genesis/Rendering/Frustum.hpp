#pragma once

namespace Genesis
{
	class Frustum
	{
	public:
		Frustum(const matrix4F& view_projection);

		bool sphereTest(vector3F& pos, float radius);

	private:
		enum Plane { Right, Left, Bottom, Top, Near, Far, Count };
		vector4F planes[Plane::Count];
	};
}
