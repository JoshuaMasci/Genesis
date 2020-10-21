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
		this->physics = new reactphysics3d::DynamicsWorld(reactphysics3d::Vector3(0.0, -9.8, 0.0));
	}

	EntityWorld::~EntityWorld()
	{
		this->onDestroy();
		delete this->physics;
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
				view.get<RigidBody>(entity).getTransform(view.get<TransformD>(entity));
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
		this->registry.destroy(entity.getHandle());
	}

	Entity EntityWorld::getEntity(EntityHandle entity_handle)
	{
		if (this->registry.valid(entity_handle))
		{
			return Entity(entity_handle, &this->registry);
		}

		return Entity(null_entity, &this->registry);
	}

	void EntityWorld::onCreate()
	{
		auto& view = this->registry.view<RigidBody>();
		for (EntityHandle entity : view)
		{
			//Build Rigid Body

		}
	}

	void EntityWorld::onDestroy()
	{

	}
}