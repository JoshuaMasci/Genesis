#pragma once

#include <entt/entity/registry.hpp>

#include "Genesis/Physics/PhysicsWorld.hpp"

namespace Genesis
{
	//Prototype
	class Application;

	class GameScene
	{
	public:
		GameScene(Application* app);
		~GameScene();

		virtual void runFrame(double delta_time);

	protected:
		Application* application;

		//EnTT
		entt::basic_registry<uint32_t> entity_registry;

		//Physics
		map<uint16_t, PhysicsWorld*> physics_worlds;

		//Game Systems
			//Pre-Frame Update
			//Per Physics tick Update
			//Frame Update
			//Render
			//Post-Frame Update
	};
}