#pragma once

#include "Genesis/Core/VectorTypes.hpp"

namespace Genesis
{
	class Camera
	{
	public:
		Camera();

		matrix4F getProjectionMatrix(vector2U& screen_size);
		matrix4F getOrthographicMatrix(float x_bounds, float y_bounds, float far_bounds);

	private:
		//Perspective varables
		float z_near = 0.1f;
		float frame_of_view;
	};
}