#pragma once

#include "Genesis/Job/JobSystem.hpp"
#include "Genesis/Input/InputManager.hpp"

#include "Genesis/Platform/Platform.hpp"
#include "Genesis/Platform/Window.hpp"
#include "Genesis/Rendering/RenderingBackend.hpp"
#include "Genesis/Rendering/RenderSystem.hpp"

#include "Genesis/Scene/SceneRenderer.hpp"

namespace Genesis
{
	class GameScene;

	class Application
	{
	public:
		Application();
		virtual ~Application();

		void run();

		virtual void update(TimeStep time_step);
		virtual void render(TimeStep time_step);

		void close();
		bool isRunning();

		//Engine Systems
		JobSystem job_system;

		InputManager input_manager;

		Platform* platform = nullptr;
		Window* window = nullptr;
		RenderingBackend* rendering_backend = nullptr;
		RenderSystem* render_system = nullptr;

		Scene scene;
		SceneRenderer* scene_renderer = nullptr;

	protected:
		bool is_running = true;
	};
};