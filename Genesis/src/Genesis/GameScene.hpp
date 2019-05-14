#pragma once

#include "Genesis/Entity.hpp"

#include "Genesis/Physics/PhysicsSystem.hpp"

namespace Genesis
{
	//Prototype
	class Application;

	//TODO switch this class to a "World class"
	class GameScene
	{
	public:
		GameScene(Application* app);
		~GameScene();

		virtual void runSimulation(double delta_time);

		virtual void drawFrame(double delta_time);

	protected:
		Application* application;

		//EnTT
		EntityRegistry entity_registry;
		
		//Physics
		PhysicsSystem physics_system;

		//Temp stuff
		EntityId temp;
		EntityId camera;

		//Game Systems
			//Pre-Frame Update
			//Simulation Update
			//Pre-Frame Update
			//Render
	};
}