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
		WorldTransform(Transform& transform = Transform())
		{
			this->current_transform = transform;
			this->previous_transform = transform;
		};

		WorldTransform(vector3D position, quaternionD orientation = quaternionD())
		: WorldTransform(Transform(position, orientation)){};

		Transform current_transform;
		Transform previous_transform;
	};
}