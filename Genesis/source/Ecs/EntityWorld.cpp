#include "Genesis/Ecs/EntityWorld.hpp"
 
#include "Genesis/Component/Hierarchy.hpp"
#include "Genesis/Component/TransformComponents.hpp"
#include "Genesis/Physics/RigidBody.hpp"

namespace Genesis
{
	EntityWorld::EntityWorld()
	{
	}

	EntityWorld::~EntityWorld()
	{
		this->onDestroy();
	}

	TransformD calcWorldTransform(EntityRegistry& registry, EntityHandle entity)
	{
		if (!registry.has<ChildNode>(entity))
		{
			//Root Node
			if (registry.has<TransformD>(entity))
			{
				TransformD& transform = registry.get<TransformD>(entity);
				return transform;
			}

			return TransformD();
		}
		ChildNode& child_node = registry.get<ChildNode>(entity);

		if (child_node.parent == null_entity || !registry.valid(child_node.parent))
		{
			return TransformD();
		}

		TransformD local_transform = TransformD();

		if (registry.has<TransformD>(entity))
		{
			local_transform = registry.get<TransformD>(entity);
		}

		return TransformUtils::transformBy(calcWorldTransform(registry, child_node.parent), local_transform);
	}

	void EntityWorld::resolveTransforms()
	{
		// Hierarchy/Transform resolve system
		// This system will resolve the world transform for all entites with a world transform
		auto& view = this->registry.view<WorldTransform>();
		for (EntityHandle entity : view)
		{
			TransformD new_transform = calcWorldTransform(registry, entity);
			view.get<WorldTransform>(entity) = (WorldTransform)new_transform;
		}
	}

	void EntityWorld::runSimulation(TimeStep time_step)
	{
		//Physics Pre-step
		{
			auto& view = this->registry.view<RigidBody, TransformD>();
			for (EntityHandle entity : view)
			{
				view.get<RigidBody>(entity).setTransform(view.get<TransformD>(entity));
			}
		}

		//Physics Step
		this->physics->update(time_step);

		//Physics Post-step
		{
			auto& view = this->registry.view<RigidBody, TransformD>();
			for (EntityHandle entity : view)
			{
				view.get<TransformD>(entity) = view.get<RigidBody>(entity).getTransform();
			}
		}
	}

	void EntityWorld::onCreate()
	{
		this->physics = new reactphysics3d::DynamicsWorld(reactphysics3d::Vector3(0.0, -9.8, 0.0));

		auto& view = this->registry.view<RigidBody, TransformD>();
		for (EntityHandle entity : view)
		{
			TransformD transform = this->registry.get<TransformD>(entity);
			this->registry.get<RigidBody>(entity).attachRigidBody(this->physics->createRigidBody(reactphysics3d::Transform(toVec3R(transform.getPosition()), toQuatR(transform.getOrientation()))));
		}

	}

	void EntityWorld::onDestroy()
	{
		auto& view = this->registry.view<RigidBody>();
		for (EntityHandle entity : view)
		{
			RigidBody& rigid_body = view.get<RigidBody>(entity);
			if (rigid_body.hasRigidBody())
			{
				this->physics->destroyRigidBody(rigid_body.removeRigidBody());
			}
		}

		delete this->physics;
		this->physics = nullptr;
	}

	void EntityWorld::clone()
	{
	}
}