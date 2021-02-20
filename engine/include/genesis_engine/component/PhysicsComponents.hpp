#pragma once

namespace Genesis
{
	enum class RigidBodyType
	{
		Static = 0,
		Kinematic = 1,
		Dynamic = 2,
	};

	enum class ShapeType
	{
		None,
		Box,
		Sphere,
		Capsule
	};

	struct RigidBodyTemplate
	{
		RigidBodyType type = RigidBodyType::Dynamic;
		double mass = 1.0f;
		bool gravity_enabled = true;
		bool is_allowed_to_sleep = true;
		vector3D linear_velocity = vector3D(0.0);
		vector3D angular_velocity = vector3D(0.0);
	};

	struct CollisionShapeTemplate
	{
		ShapeType type = ShapeType::None;
		union
		{
			vector3D box_size;
			double sphere_radius;
			vector2D capsule_size;
		} type_data;
	};

	struct TriggerShapeTemplate
	{
		ShapeType type = ShapeType::None;
		union
		{
			vector3D box_size;
			double sphere_radius;
			vector2D capsule_size;
		} type_data;
	};
}