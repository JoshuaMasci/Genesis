#pragma once

#include "Genesis/Job/JobSystem.hpp"
#include "Genesis/Input/InputManager.hpp"

#include "Genesis/Platform/Platform.hpp"
#include "Genesis/Platform/Window.hpp"
#include "Genesis/RenderingBackend/RenderingBackend.hpp"
#include "Genesis/Rendering/RenderSystem.hpp"
#include "Genesis/Rendering/ImGuiRenderer.hpp"

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
		virtual void render(TimeStep interpolation_value);

		bool startFrame();
		void endFrame();

		void close();
		bool isRunning();

		//Engine Systems
		JobSystem job_system;

		InputManager input_manager;

		Platform* platform = nullptr;
		Window* window = nullptr;
		RenderingBackend* rendering_backend = nullptr;
		RenderSystem* render_system = nullptr;

		ImGuiRenderer* ui_renderer = nullptr;
		
	protected:
		bool is_running = true;
	};
};