#pragma once

namespace genesis
{
	class JobSystem;
	class InputManager;
	class Platform;
	class Window;
	class RenderingBackend;

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
		JobSystem* job_system = nullptr;
		InputManager* input_manager = nullptr;
		Platform* platform ;
		Window* window = nullptr;
		RenderingBackend* rendering_backend = nullptr;
		
	protected:
		bool is_running = true;
	};
};