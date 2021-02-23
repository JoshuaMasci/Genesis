#pragma once

namespace genesis
{
	class Frustum
	{
	public:
		Frustum(const mat4f& view_projection);

		bool sphereTest(vec3f& pos, float radius);

	private:
		enum Plane { Right, Left, Bottom, Top, Near, Far, Count };
		vec4f planes[Plane::Count];
	};
}
