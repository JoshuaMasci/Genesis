#include "TransformSystem.hpp"

#include "Genesis/Debug/Log.hpp"
#include "Genesis/Debug/Profiler.hpp"

#include "glm/gtx/compatibility.hpp"

using namespace Genesis;

TransformD WorldTransform::linearInterpolation(double interpolation_value)
{
	TransformD result;
	result.setPosition(glm::lerp(this->current.getPosition(), this->previous.getPosition(), interpolation_value));
	result.setOrientation(glm::lerp(this->current.getOrientation(), this->previous.getOrientation(), interpolation_value));
	result.setScale(this->current.getScale());
	return result;
}

void TransformSystem::preSimulation(EntityRegistry& world, JobSystem* job_system)
{
	GENESIS_PROFILE_FUNCTION("TransformSystem::preSimulation");
	auto view = world.view<WorldTransform>();
	for (auto entity : view)
	{
		WorldTransform& world_transform = view.get<WorldTransform>(entity);
		world_transform.previous = world_transform.current;
	}
}

void tempUpdateChild(EntityRegistry& world, EntityId child, TransformD& parent_transform)
{
	if (world.has<WorldTransform, ChildNode, ChildTransform>(child))
	{
		WorldTransform& world_transform = world.get<WorldTransform>(child);
		ChildNode& child_node = world.get<ChildNode>(child);
		ChildTransform& child_transform = world.get<ChildTransform>(child);

		world_transform.current = TransformUtil::transformBy(parent_transform, child_transform.local_transform);

		for (auto entity : child_node.child_entities)
		{
			tempUpdateChild(world, entity, world_transform.current);
		}
	}
	else
	{
		GENESIS_ENGINE_ERROR("TransformSystem::tempUpdateChild child doesn't have node");
	}
}

void TransformSystem::calculateHierarchy(EntityRegistry& world, JobSystem* job_system)
{
	GENESIS_PROFILE_FUNCTION("TransformSystem::calculateHierarchy");

	auto view = world.view<WorldTransform, ParentNode>();
	for (auto entity : view)
	{
		WorldTransform& world_transform = view.get<WorldTransform>(entity);
		ParentNode& parent_node = view.get<ParentNode>(entity);
	
		for (auto entity : parent_node.child_entities)
		{
			tempUpdateChild(world, entity, world_transform.current);
		}
	}
}
