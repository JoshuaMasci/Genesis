#pragma once

#include "Genesis/Ecs/Ecs.hpp"

namespace Genesis
{
	namespace Experimental
	{
		class RaycastSystem
		{
		public:
			static bool runRaycast(EntityRegistry* registry, vector3D ray_start, vector3D ray_direction, double ray_distance);
		};

		class UpdateCollisionShapeSystem
		{
		public:
			static void run(EntityRegistry* registry);
		};
	}
}