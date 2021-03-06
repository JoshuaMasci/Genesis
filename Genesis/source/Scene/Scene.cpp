#include "Genesis/Scene/Scene.hpp"

#include "Genesis/Component/NameComponent.hpp"
#include "Genesis/Scene/Entity.hpp"

//TEMP
#include "Genesis/Component/TransformComponent.hpp"
#include "Genesis/Physics/RigidBody.hpp"
#include "Genesis/Physics/CollisionShape.hpp"
#include "Genesis/Physics/PhysicsWorld.hpp"
namespace Genesis
{
	Scene::Scene()
	{
		this->scene_components = SceneComponents(&this->registry, this->registry.create());
		this->scene_components.add<NameComponent>("Scene Components");
	}

	Entity Scene::createEntity(const char* name)
	{
		Entity entity = Entity(this, this->registry.create());
		entity.add<NameComponent>(name);
		return entity;
	}

	void Scene::destoryEntity(Entity entity)
	{
		for (EntityHandle child : entity.children())
		{
			this->destoryEntity(Entity(this, child));
		}

		this->registry.destroy(entity.handle());
	}

	void Scene::addChild(Entity parent, Entity child)
	{
		HierarchyUtils::addChild(this->registry, parent.handle(), child.handle());
	}

	void Scene::removeChild(Entity parent, Entity child)
	{
		HierarchyUtils::removeChild(this->registry, parent.handle(), child.handle());
	}

	void Scene::initialize_scene()
	{
		if (this->scene_components.has<PhysicsWorld>())
		{
			PhysicsWorld& physics_world = this->scene_components.get<PhysicsWorld>();

			auto& view = this->registry.view<RigidBody, Transform>(entt::exclude<ChildNode>);
			for (EntityHandle entity : view)
			{
				RigidBody& rigid_body = view.get<RigidBody>(entity);
				rigid_body.attachRigidBody(physics_world.createRigidBody(view.get<Transform>(entity)));

				if (this->registry.has<CollisionShape>(entity))
				{
					CollisionShape& shape = this->registry.get<CollisionShape>(entity);
					shape.shape = CollisionShape::createCollisionShape(shape);

					if (shape.shape != nullptr)
					{
						shape.proxy = rigid_body.addShape(shape.shape, TransformD(), shape.shape_mass);
					}
				}
			}
		}
	}

	void Scene::deinitialize_scene()
	{
		if (this->scene_components.has<PhysicsWorld>())
		{
			PhysicsWorld& physics_world = this->scene_components.get<PhysicsWorld>();

			auto& view = this->registry.view<RigidBody>();
			for (EntityHandle entity : view)
			{
				reactphysics3d::RigidBody* rigid_body = view.get<RigidBody>(entity).removeRigidBody();

				if (rigid_body != nullptr)
				{
					if (this->registry.has<CollisionShape>(entity))
					{
						CollisionShape& shape = this->registry.get<CollisionShape>(entity);
						rigid_body->removeCollisionShape(shape.proxy);
						delete shape.shape;
						shape.shape = nullptr;
					}

					physics_world.deleteRigidBody(rigid_body);
				}
			}
		}
	}

}