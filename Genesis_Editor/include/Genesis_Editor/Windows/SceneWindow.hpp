#pragma once

#include "Genesis/Ecs/EntityWorld.hpp"

#include <Genesis/Input/InputManager.hpp>
#include <Genesis/LegacyBackend/LegacyBackend.hpp>
#include <Genesis/LegacyRendering/LegacyWorldRenderer.hpp>

namespace Genesis
{
	class SceneWindow
	{
	public:
		SceneWindow(InputManager* input_manager, LegacyBackend* legacy_backend);
		~SceneWindow();

		void update(TimeStep time_step);

		void draw(EntityWorld& world);

		bool isSceneRunning() { return this->is_scene_running; };

		TransformD getSceneCameraTransform() { return this->scene_camera_transform; };

	private:
		bool is_window_active = false;

		bool is_scene_running = false;

		InputManager* input_manager = nullptr;

		LegacyBackend* legacy_backend = nullptr;
		LegacyWorldRenderer* world_renderer = nullptr;

		Framebuffer framebuffer = nullptr;
		vector2U framebuffer_size = vector2U(0);

		Camera scene_camera;
		TransformD scene_camera_transform;

		EntityHandle override_camera = null_entity;
		EntityHandle override_camera_node = null_entity;

		//meters per second
		double linear_speed = 2.0;
		//rads per second
		double angular_speed = PI_D / 8.0;
	};
}