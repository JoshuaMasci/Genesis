#pragma once

#include "Genesis/Ecs/EscWorld.hpp"
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
		vector<EntityHandle> child_entities;
	};

	struct ChildTransform
	{
		TransformF local_transform;
	};

	struct ChildNode
	{
		EntityHandle root_parent;
		EntityHandle parent;
		vector<EntityHandle> child_entities;
	};

	class TransformSystem
	{
	public:
		static void preSimulation(EcsWorld& world, JobSystem* job_system);
		static void calculateHierarchy(EcsWorld& world, JobSystem* job_system);
	};
}