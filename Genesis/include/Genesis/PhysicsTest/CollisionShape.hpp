#pragma once

namespace Genesis
{
	namespace Experimental
	{
		enum class CollisionShapeType
		{
			None,
			Box,
			Sphere,
			Capsule
		};

		struct CollisionShape
		{
			/*CollisionShapeType type = CollisionShapeType::None;
			union
			{
				vector3D box_size;
				double sphere_radius;
				vector2D capsule_size;
			} type_data;*/

			double sphere_radius;

			TransformD world_transform;
		};
	}
}