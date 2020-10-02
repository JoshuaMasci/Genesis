#include "Genesis/Ecs/EntityWorld.hpp"
 
#include "Genesis/Component/Hierarchy.hpp"
#include "Genesis/Physics/RigidBody.hpp"
#include "Genesis/Component/NameComponent.hpp"
#include "Genesis/Component/ModelComponent.hpp"

namespace Genesis
{
	EntityWorld::EntityWorld()
	{
	}

	EntityWorld::~EntityWorld()
	{
		this->onDestroy();

		auto& view = this->registry.view<ModelComponent>();
		for (EntityHandle entity : view)
		{
			ModelComponent& model = view.get<ModelComponent>(entity);
			size_t count = model.mesh.use_count();
			model.mesh.reset();
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

	Entity EntityWorld::createEntity(const string& name)
	{
		EntityHandle handle = this->registry.create();
		this->registry.assign<NameComponent>(handle, name.c_str());
		return Entity(handle, &this->registry);
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