#pragma once

#include "Genesis/Entity/Entity.hpp"
#include "Genesis/Core/Transform.hpp"
#include "Genesis/Core/Types.hpp"
#include "Genesis/Job/JobSystem.hpp"

namespace Genesis
{
	struct WorldTransform
	{
		WorldTransform(TransformD transform);

		TransformD current;
		TransformD previous;

		TransformD linearInterpolation(TimeStep interpolation_value);
	};

	struct ParentNode
	{
		vector<EntityId> child_entities;
	};

	struct ChildTransform
	{
		TransformF local_transform;
	};

	struct ChildNode
	{
		EntityId root_parent;
		EntityId parent;
		vector<EntityId> child_entities;
	};

	class TransformSystem
	{
	public:
		static void preSimulation(EntityRegistry& world, JobSystem* job_system);
		static void calculateHierarchy(EntityRegistry& world, JobSystem* job_system);
	};
}