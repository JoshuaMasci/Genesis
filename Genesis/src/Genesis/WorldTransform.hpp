#pragma once

#include "Genesis/Core/Transform.hpp"

namespace Genesis
{
	/*
		This struct is the actual transform class used by entities
		Stores Current Transform, Previous Transform, and
		may allow for interpolation at render time
	*/
	struct WorldTransform
	{
		WorldTransform(TransformD& transform = TransformD())
		{
			this->current_transform = transform;
			this->previous_transform = transform;
		};

		WorldTransform(vector3D position, quaternionD orientation = quaternionD(1.0, 0.0, 0.0, 0.0))
		: WorldTransform(TransformD(position, orientation)){};

		TransformD current_transform;
		TransformD previous_transform;
	};
}