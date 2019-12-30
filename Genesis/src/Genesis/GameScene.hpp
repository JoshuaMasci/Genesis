#pragma once

#include "Genesis/Entity.hpp"

#include "Genesis/Rendering/Renderer.hpp"
#include "Genesis/Rendering/ImGuiRenderer.hpp"
#include "Genesis/Rendering/Lighting.hpp"

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

		virtual void drawWorld(double delta_time);

	protected:
		Application* application;

		//EnTT
		EntityRegistry entity_registry;
		
		//Physics
		//PhysicsSystem physics_system;

		//Rendering
		Renderer* renderer = nullptr;
		ImGuiRenderer* ui_renderer = nullptr;

		Shader screen_shader = nullptr;
		List<UniformBuffer> uniform_buffers;


		//Temp stuff
		EntityId temp;
		EntityId camera;
		EntityId directional_light;

		//Game Systems
			//Pre-Frame Update
			//Simulation Update
			//Pre-Frame Update
			//Render
	};
}