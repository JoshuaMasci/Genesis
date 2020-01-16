#pragma once

#include "Genesis/Core/Types.hpp"

namespace Genesis
{
	struct Camera
	{
		Camera(float fov)
		{
			this->frame_of_view = fov;
		};

		float frame_of_view = 75.0f;
		float z_near = 0.01f;
	};
}