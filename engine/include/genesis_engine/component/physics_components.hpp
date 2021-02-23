#pragma once

namespace genesis
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
		vec3d linear_velocity = vec3d(0.0);
		vec3d angular_velocity = vec3d(0.0);
	};

	struct CollisionShapeTemplate
	{
		ShapeType type = ShapeType::None;
		union
		{
			vec3d box_size;
			double sphere_radius;
			vec2d capsule_size;
		} type_data;
	};

	struct TriggerShapeTemplate
	{
		ShapeType type = ShapeType::None;
		union
		{
			vec3d box_size;
			double sphere_radius;
			vec2d capsule_size;
		} type_data;
	};
}