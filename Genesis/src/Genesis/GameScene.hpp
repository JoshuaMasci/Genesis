#pragma once

#include "Genesis/Entity.hpp"
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
		EntityRegistry entity_registry;

		//Physics
		map<uint16_t, PhysicsWorld*> physics_worlds;
		
		//Temp stuff
		PhysicsWorld* world;
		EntityId temp;

		//Game Systems
			//Pre-Frame Update
			//Per Physics tick Update
			//Frame Update
			//Render
			//Post-Frame Update
	};
}