#include "Genesis/Entity/World.hpp"

#include "Genesis/Entity/Entity.hpp"

#include "Genesis/Entity/DebugCamera.hpp"
#include "Genesis/Physics/RigidBody.hpp"

#include "Genesis/Rendering/Camera.hpp"
#include "Genesis/Rendering/Lights.hpp"

#include "Genesis/LegacyRendering/LegacyMeshRenderer.hpp"

#include "Genesis/Entity/NameComponent.hpp"

using namespace Genesis;
using namespace Genesis::Physics;

struct CharacterController
{
	CharacterController(double force)
	{
		this->forward_force = force;
		this->sideways_force = force;
	};

	double forward_force;
	double sideways_force;

	double forward_input;
	double sideways_input;
};

World::World(BaseWorldRenderer* world_renderer)
{
	this->world_renderer = world_renderer;

	this->entity_registry = new EntityRegistry();
	this->physics_world = new PhysicsWorld(vector3D(0.0, -9.8, 0.0));

	{
		this->main_camera = this->entity_registry->create();
		this->entity_registry->assign<TransformD>(this->main_camera, vector3D(0.0, 0.0, -20.0));
		this->entity_registry->assign<Camera>(this->main_camera, 77.0f);
		this->entity_registry->assign<DebugCamera>(this->main_camera, 5.0, 0.3);
		this->entity_registry->assign<NameComponent>(this->main_camera, "Main_Camera");
	}

	{
		EntityHandle entity = this->entity_registry->create();
		this->entity_registry->assign<TransformD>(entity, vector3D(0.0, 0.0, 0.0));
		this->entity_registry->assign<RigidBody>(entity, this->physics_world->addRigidBody(this->entity_registry->get<TransformD>(entity)));
		this->entity_registry->assign<ProxyShape>(entity, this->entity_registry->get<RigidBody>(entity).addCollisionShape(new reactphysics3d::SphereShape(1.0f), TransformD(), 1.0f));

		this->entity_registry->assign<LegacyMeshComponent>(entity, "res/sphere.obj");
		this->entity_registry->assign<LegacyMaterialComponent>(entity, "res/materials/red.csv");


		this->entity_registry->get<RigidBody>(entity).get()->setAngularDamping(1.0);
	}

	{
		EntityHandle entity = this->entity_registry->create();
		this->entity_registry->assign<TransformD>(entity, vector3D(0.0, -5.0, 0.0));
		this->entity_registry->assign<StaticRigidBody>(entity, this->physics_world->addRigidBody(this->entity_registry->get<TransformD>(entity)));
		this->entity_registry->assign<ProxyShape>(entity, this->entity_registry->get<StaticRigidBody>(entity).addCollisionShape(new reactphysics3d::BoxShape(reactphysics3d::Vector3(16.0, 1.0, 16.0)), TransformD(), 0.0f));

		this->entity_registry->assign<LegacyMeshComponent>(entity, "res/ground.obj");
		this->entity_registry->assign<LegacyMaterialComponent>(entity, "res/materials/grid.csv");

	}

	{
		EntityHandle entity = this->entity_registry->create();
		this->entity_registry->assign<TransformD>(entity, vector3D(0.0, 0.0, 0.0), glm::angleAxis(glm::radians(85.0f), vector3F(1.0f, 0.0f, 0.0f)));
		this->entity_registry->assign<DirectionalLight>(entity, vector3F(1.0f), 0.2f);
		this->entity_registry->assign<PointLight>(entity, 25.0f, vector2F(1.0f, 0.0f), vector3F(0.8f, 0.0f, 0.8f), 0.2f);
	}


	{
		Entity* entity_1 = new Entity(1);

		Entity* entity_2 = new Entity(2);
		entity_2->setLocalPosition(vector3D(0.0, 1.0, 0.0));

		Entity* entity_3 = new Entity(3);
		entity_3->setLocalPosition(vector3D(1.0, 1.0, 0.0));

		Entity* entity_4 = new Entity(4);
		entity_4->setLocalPosition(vector3D(0.0, 1.0, 0.0));

		entity_1->addChild(entity_2);
		entity_1->addChild(entity_3);

		entity_2->addChild(entity_4);

		this->addEntity(entity_1);

		Entity* ladder_0 = new Entity(5 + 0, "Ladder_0");
		ladder_0->setLocalPosition(vector3D(-1.0, 0.0, 0.0));

		Entity* ladder_1 = new Entity(5 + 1, "Ladder_1");
		ladder_1->setLocalPosition(vector3D(0.0, 1.0, 0.0));

		Entity* ladder_2 = new Entity(5 + 2, "Ladder_2");
		ladder_2->setLocalPosition(vector3D(0.0, 1.0, 0.0));

		Entity* ladder_3 = new Entity(5 + 3, "Ladder_3");
		ladder_3->setLocalPosition(vector3D(0.0, 1.0, 0.0));

		Entity* ladder_4 = new Entity(5 + 4, "Ladder_4");
		ladder_4->setLocalPosition(vector3D(0.0, 1.0, 0.0));

		ladder_0->addChild(ladder_1);
		ladder_1->addChild(ladder_2);
		ladder_2->addChild(ladder_3);
		ladder_3->addChild(ladder_4);

		this->addEntity(ladder_0);
	}
}

World::~World()
{
	for (Entity* entity : this->root_entities)
	{
		entity->removeFromWorld();
		delete entity;
	}

	delete this->entity_registry;
	delete this->physics_world;
}

void World::runSimulation(Application* application, TimeStep time_step)
{
	GENESIS_PROFILE_FUNCTION("World::runSimulation");
	physics_world->simulate(time_step);

	//Physics System
	{
		auto& view = this->entity_registry->view<RigidBody, TransformD>();
		for (EntityHandle entity : view)
		{
			RigidBody& rigid_body = view.get<RigidBody>(entity);
			TransformD& transform = view.get<TransformD>(entity);
			TransformD rigid_body_transform = rigid_body.getTransform();
			transform.setPosition(rigid_body_transform.getPosition());
			transform.setOrientation(rigid_body_transform.getOrientation());
		}
	}

	//Character Systems
	{
		auto& view = this->entity_registry->view<CharacterController>();
		for (EntityHandle entity : view)
		{
			CharacterController& character = view.get<CharacterController>(entity);
			character.forward_input = application->input_manager->getButtonAxisCombo("Debug_ForwardBackward", "Debug_Forward", "Debug_Backward");
			character.sideways_input = application->input_manager->getButtonAxisCombo("Debug_LeftRight", "Debug_Left", "Debug_Right");
		}
	}
	{
		auto& view = this->entity_registry->view<RigidBody, CharacterController>();
		for (EntityHandle entity : view)
		{
			RigidBody& rigid_body = view.get<RigidBody>(entity);
			CharacterController& character = view.get<CharacterController>(entity);
			TransformD transform = rigid_body.getTransform();

			vector3D old_velocity = rigid_body.getLinearVelocity();

			vector3D velocity((transform.getForward() * character.forward_force * character.forward_input) + (transform.getLeft() * character.sideways_force * character.sideways_input));
			velocity.y = old_velocity.y;
			rigid_body.setLinearVelocity(velocity);
		}
	}

	DebugCamera::update(application->input_manager, this->entity_registry->get<DebugCamera>(this->main_camera), this->entity_registry->get<TransformD>(this->main_camera), time_step);


	for (Entity* entity : this->root_entities)
	{
		entity->updateTransform(false);
	}
}

void World::addEntity(Entity* entity)
{
	GENESIS_ENGINE_ASSERT_ERROR((entity != nullptr), "world tried to add a null child");
	GENESIS_ENGINE_ASSERT_ERROR((entity->parent == nullptr && entity->world == nullptr), ("{}:{} already has a parent", entity->id, entity->name));
	
	entity->addtoWorld(this);
	this->root_entities.push_back(entity);
}

void World::removeEntity(Entity* entity)
{
	GENESIS_ENGINE_ASSERT_ERROR((entity != nullptr), "world tried to remove a null entity");

	for (size_t i = 0; i < this->root_entities.size(); i++)
	{
		if (this->root_entities[i] == entity)
		{
			size_t last_index = this->root_entities.size() - 1;
			if (i != last_index)
			{
				this->root_entities[i] = this->root_entities[last_index];
			}
			this->root_entities.pop_back();
			entity->removeFromWorld();
			return;
		}
	}

	GENESIS_ENGINE_ERROR("{}:{} is not in world", entity->id, entity->name);
}
