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

	class CollisionShape
	{
	protected:
		reactphysics3d::CollisionShape* shape = nullptr;
		reactphysics3d::ProxyShape* proxy = nullptr;
		reactphysics3d::RigidBody* rigid_body = nullptr;

	public:

		CollisionShapeType type = CollisionShapeType::None;
		union
		{
			vector3D box_size;
			double sphere_radius;
			vector2D capsule_size;
		} type_data;

		void onAttach();
		void onDetach();
	};
}