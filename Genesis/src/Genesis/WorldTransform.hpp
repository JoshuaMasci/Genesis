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

		Transform current_transform;
		Transform previous_transform;
	};
}