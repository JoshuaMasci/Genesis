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
		return Entity(&this->registry, handle);
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
			return Entity(&this->registry, entity_handle);
		}

		return Entity(&this->registry, null_entity);
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

			if (this->registry.has<CollisionShape>(entity))
			{
				CollisionShape& shape = this->registry.get<CollisionShape>(entity);
				shape.shape = CollisionShape::createCollisionShape(shape);

				if (shape.shape != nullptr)
				{
					shape.proxy = rigidbody.addShape(shape.shape, TransformD(), shape.shape_mass);
				}
			}
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