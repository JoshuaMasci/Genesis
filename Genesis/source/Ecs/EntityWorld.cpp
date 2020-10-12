#include "Genesis/Ecs/EntityWorld.hpp"
 
#include "Genesis/Component/Hierarchy.hpp"
#include "Genesis/Physics/RigidBody.hpp"
#include "Genesis/Physics/CollisionShape.hpp"
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
		/*{
			auto& view = this->registry.view<RigidBody, TransformD>();
			for (EntityHandle entity : view)
			{
				view.get<TransformD>(entity) = view.get<RigidBody>(entity).getTransform();
			}
		}*/
	}

	void EntityWorld::updateTransforms()
	{
		//Tree Update
		this->registry.each([&](auto entity)
		{
			if (!registry.has<ChildNode>(entity))
			{
				this->updateEntityTreeRoot(entity);
			}
		});
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

		auto& rigidbody_view = this->registry.view<RigidBody, TransformD>(entt::exclude_t<ChildNode>());
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
					shape.proxy = rigidbody.addShape(shape.shape, TransformD());
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

	void EntityWorld::updateEntityTreeRoot(EntityHandle entity)
	{
		if (!registry.has<TransformD>(entity))
		{
			//For rignt now all entities need a transform
			//TODO Remove maybe
			return;
		}

		TransformD& world_transform = registry.get<TransformD>(entity);

		{
			RigidBody* rigidbody = this->registry.try_get<RigidBody>(entity);
			if (rigidbody != nullptr)
			{
				world_transform = rigidbody->getTransform();
			}
		}

		//Component Updates
		{
			ModelComponent* model_component = this->registry.try_get<ModelComponent>(entity);
			if (model_component != nullptr)
			{
				model_component->world_transform = world_transform;
			}
		}

		//Hierarchy update
		{
			ParentNode* parent_node = this->registry.try_get<ParentNode>(entity);
			if (parent_node != nullptr)
			{
				EntityHandle child = parent_node->first;
				while (this->registry.valid(child))
				{
					this->updateEntityTree(child, world_transform, TransformD()); //Root has no relative_transform to it's self

					ChildNode* child_node = this->registry.try_get<ChildNode>(child);
					if (child_node != nullptr)
					{
						child = child_node->next;
					}
					else
					{
						child = null_entity;
					}
				}
			}
		}
	}

	void EntityWorld::updateEntityTree(EntityHandle entity, const TransformD& parent_world_transform, const TransformD& parent_relative_transform)
	{
		if (!registry.has<TransformD>(entity))
		{
			//For rignt now all entities need a transform
			//TODO Remove maybe
			return;
		}

		TransformD& local_transform = this->registry.get<TransformD>(entity);
		TransformD world_transform;
		TransformD relative_transform;
		TransformUtils::transformByInplace(world_transform, parent_world_transform, local_transform);
		TransformUtils::transformByInplace(relative_transform, parent_relative_transform, local_transform);

		//Component Updates
		{
			ModelComponent* model_component = this->registry.try_get<ModelComponent>(entity);
			if (model_component != nullptr)
			{
				model_component->world_transform = world_transform;
			}
		}

		//Hierarchy update
		{
			ParentNode* parent_node = this->registry.try_get<ParentNode>(entity);
			if (parent_node != nullptr)
			{
				EntityHandle child = parent_node->first;
				while (this->registry.valid(child))
				{
					this->updateEntityTree(child, world_transform, relative_transform); //Root has no relative_transform to it's self

					ChildNode* child_node = this->registry.try_get<ChildNode>(child);
					if (child_node != nullptr)
					{
						child = child_node->next;
					}
					else
					{
						child = null_entity;
					}
				}
			}
		}
	}
}