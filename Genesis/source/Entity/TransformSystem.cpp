#include "Genesis/Entity/TransformSystem.hpp"

#include "Genesis/Debug/Log.hpp"
#include "Genesis/Debug/Profiler.hpp"

#include "glm/gtx/compatibility.hpp"

using namespace Genesis;

WorldTransform::WorldTransform(TransformD transform)
{
	this->current = transform;
	this->previous = transform;
}

TransformD WorldTransform::linearInterpolation(TimeStep interpolation_value)
{
	TransformD result;
	result.setPosition(glm::lerp(this->current.getPosition(), this->previous.getPosition(), interpolation_value));
	result.setOrientation(glm::lerp(this->current.getOrientation(), this->previous.getOrientation(), interpolation_value));
	result.setScale(this->current.getScale());
	return result;
}

void preSimulationThread(uint32_t thread_id, View view)
{
	GENESIS_PROFILE_FUNCTION("TransformSystem::preSimulationThread");

	for (size_t i = 0; i < view.getSize(); i++)
	{
		WorldTransform* world_transform = view.getComponent<WorldTransform>(i);
		world_transform->previous = world_transform->current;
	}
}

void TransformSystem::preSimulation(EcsWorld& world, JobSystem* job_system)
{
	GENESIS_PROFILE_FUNCTION("TransformSystem::preSimulation");

	JobCounter counter = 0;

	std::vector<View> views = world.getView<WorldTransform>();
	for (auto& view : views)
	{
		job_system->addJob(std::bind(preSimulationThread, std::placeholders::_1, view), &counter);
	}

	JobSystem::waitForCounter(counter);
}

void tempUpdateChild(EcsWorld& world, EntityHandle child, TransformD& parent_transform)
{
	/*if (world.has)
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
	}*/
}

void TransformSystem::calculateHierarchy(EcsWorld& world, JobSystem* job_system)
{
	GENESIS_PROFILE_FUNCTION("TransformSystem::calculateHierarchy");

	std::vector<View> views = world.getView<WorldTransform, ParentNode>();
	for (auto& view : views)
	{
		for (size_t i = 0; i < view.getSize(); i++)
		{
			EntityHandle entity = view.get(i);
			WorldTransform* world_transform = view.getComponent<WorldTransform>(entity);
			ParentNode* parent_node = view.getComponent<ParentNode>(entity);

			for (auto entity : parent_node->child_entities)
			{
				tempUpdateChild(world, entity, world_transform->current);
			}
		}
	}
}
