#pragma once

#include <Genesis/ECS/EntityRegistry.hpp>
#include <Genesis/Input/InputManager.hpp>
#include <Genesis/LegacyBackend/LegacyBackend.hpp>
#include <Genesis/LegacyRendering/LegacyWorldRenderer.hpp>

namespace Genesis
{
	class SceneViewWindow
	{
	public:
		SceneViewWindow(InputManager* input_manager, LegacyBackend* legacy_backend);
		~SceneViewWindow();

		void udpate(TimeStep time_step);

		void drawWindow(EntityWorld* world);

	private:
		bool is_window_active = false;

		InputManager* input_manager = nullptr;

		LegacyBackend* legacy_backend = nullptr;
		LegacyWorldRenderer* world_renderer = nullptr;

		Framebuffer framebuffer = nullptr;
		Framebuffer gamma_correct_framebuffer = nullptr;
		vector2U framebuffer_size = vector2U(0);

		Camera scene_camera;
		TransformD scene_camera_transform;

		//meters per second
		double linear_speed = 2.0;
		//rads per second
		double angular_speed = PI_D / 8.0;
	};
}