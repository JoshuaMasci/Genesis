#include "Genesis/PhysicsTest/PhysicsTestSystems.hpp"

#include "Genesis/PhysicsTest/CollisionShape.hpp"

bool Genesis::Experimental::RaycastSystem::runRaycast(EntityRegistry* registry, vector3D ray_start, vector3D ray_direction, double ray_distance)
{
	double ray_length = ray_distance * ray_distance;

	auto collision_shape_view = registry->view<Experimental::CollisionShape>();

	for (EntityHandle entity : collision_shape_view)
	{
		Experimental::CollisionShape& shape = collision_shape_view.get<Experimental::CollisionShape>(entity);

		const vector3D sphere_pos = shape.world_transform.getPosition();
		const double sphere_radius = shape.sphere_radius * shape.sphere_radius;

		vector3D ray_to_sphere = sphere_pos - ray_start;
		double ray_closest_length = glm::dot(ray_to_sphere, ray_direction);

		if (ray_closest_length < 0.0)
		{
			break;
		}

		vector3D ray_closet_point = ray_start + (ray_direction * ray_closest_length);
		
		double ray_closet_point_dist = glm::length2(sphere_pos - ray_closet_point);

		if (ray_closet_point_dist <= sphere_radius)
		{
			double dist_to_entry = sqrt(sphere_radius - ray_closet_point_dist);
			double entry_point = ray_closest_length - dist_to_entry;

			if (entry_point <= ray_distance && entry_point >= 0.0)
			{
				return true;//Hit
			}
		}

	}

	return false;
}

void Genesis::Experimental::UpdateCollisionShapeSystem::run(EntityRegistry * registry)
{
	//Right now we assume no hierachy
	auto collision_shape_view = registry->view<Experimental::CollisionShape, TransformD>();

	for (EntityHandle entity : collision_shape_view)
	{
		Experimental::CollisionShape& shape = collision_shape_view.get<Experimental::CollisionShape>(entity);
		shape.world_transform = collision_shape_view.get<TransformD>(entity);
	}
}
