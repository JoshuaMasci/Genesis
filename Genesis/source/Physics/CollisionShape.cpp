#include "Genesis/Physics/CollisionShape.hpp"

namespace Genesis
{
	reactphysics3d::CollisionShape* CollisionShape::createCollisionShape(const CollisionShape& shape)
	{
		switch (shape.type)
		{
		case Genesis::CollisionShapeType::Box:
			return new reactphysics3d::BoxShape(toVec3R(shape.type_data.box_size));
		case Genesis::CollisionShapeType::Sphere:
			return new reactphysics3d::SphereShape(shape.type_data.sphere_radius);
		case Genesis::CollisionShapeType::Capsule:
			return new reactphysics3d::CapsuleShape(shape.type_data.capsule_size.x, shape.type_data.capsule_size.y);
		default:
			return nullptr;
		}
	}
}