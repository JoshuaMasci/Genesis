#pragma once

#include "Genesis/Input/InputManager.hpp"
#include "Genesis/LegacyBackend/LegacyBackend.hpp"
#include "Genesis/LegacyRendering/LegacySceneRenderer.hpp"

#include "Genesis/Scene/Scene.hpp"
#include "Genesis/Scene/Entity.hpp"

namespace Genesis
{
	class SceneWindow
	{
	public:
		SceneWindow(InputManager* input_manager, LegacyBackend* legacy_backend);
		~SceneWindow();

		void update(TimeStep time_step);
		void draw(SceneRenderList& render_list, SceneLightingSettings& lighting, Entity selected_entity = Entity());

		TransformD get_scene_camera_transform() { return this->scene_camera_transform; };

	private:
		bool window_active = false;

		InputManager* input_manager = nullptr;

		LegacyBackend* legacy_backend = nullptr;
		LegacySceneRenderer* world_renderer = nullptr;

		Framebuffer framebuffer = nullptr;
		vector2U framebuffer_size = vector2U(0);

		Camera scene_camera;
		TransformD scene_camera_transform;

		//meters per second
		double linear_speed = 2.0;
		//rads per second
		double angular_speed = PI_D / 8.0;
		RenderSettings settings;
	};
}