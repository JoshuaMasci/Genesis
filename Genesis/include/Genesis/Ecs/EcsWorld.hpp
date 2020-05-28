#pragma once

#include "Genesis/Physics/PhysicsWorld.hpp"
#include "Genesis/Ecs/Entt.hpp"

namespace Genesis
{
	struct EcsWorld
	{
		EntityRegistry entity_registry;
		PhysicsWorld* physics_world = nullptr;
		EntityHandle main_camera;
	};
}