#include "Genesis/Ecs/EntityWorld.hpp"
 
#include "Genesis/Physics/RigidBody.hpp"
#include "Genesis/Physics/CollisionShape.hpp"
#include "Genesis/Component/NameComponent.hpp"
#include "Genesis/Component/ModelComponent.hpp"
#include "Genesis/Component/NodeComponent.hpp"

namespace Genesis
{
	EntityWorld::EntityWorld()
	{
	}

	EntityWorld::~EntityWorld()
	{
		this->onDestroy();
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

	void EntityWorld::resolveTransforms()
	{
		//Node Transform Update
		{
			auto& view = this->registry.view<NodeComponent>();
			for (EntityHandle entity : view)
			{
				NodeSystem::updateTransform(this->registry.get<NodeComponent>(entity));
			}
		}
	}

	Entity EntityWorld::createEntity()
	{
		EntityHandle handle = this->registry.create();
		return Entity(handle, &this->registry);
	}

	Entity EntityWorld::createEntity(const string& name)
	{
		Entity entity = this->createEntity();
		entity.addComponent<NameComponent>(name.c_str());
		return entity;
	}

	void EntityWorld::destroyEntity(Entity entity)
	{
		//TODO Implement
	}

	void EntityWorld::onCreate()
	{
		this->physics = new reactphysics3d::DynamicsWorld(reactphysics3d::Vector3(0.0, -9.8, 0.0));

		auto& rigidbody_view = this->registry.view<RigidBody, TransformD>();
		for (EntityHandle entity : rigidbody_view)
		{
			TransformD& transform = rigidbody_view.get<TransformD>(entity);
			RigidBody& rigidbody = rigidbody_view.get<RigidBody>(entity);
			rigidbody.attachRigidBody(this->physics->createRigidBody(reactphysics3d::Transform(toVec3R(transform.getPosition()), toQuatR(transform.getOrientation()))));
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
}