#pragma once

#include "Genesis/Physics/ReactPhyscis.hpp"

namespace Genesis
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
		reactphysics3d::CollisionShape* shape = nullptr;
		reactphysics3d::ProxyShape* proxy = nullptr;

		double shape_mass = 1.0f;
		CollisionShapeType type = CollisionShapeType::None;
		union
		{
			vector3D box_size;
			double sphere_radius;
			vector2D capsule_size;
		} type_data;

		static reactphysics3d::CollisionShape* createCollisionShape(const CollisionShape& shape);
	};
}