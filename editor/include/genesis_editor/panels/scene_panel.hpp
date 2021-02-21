#pragma once

#include "genesis_engine/input/input_manager.hpp"
#include "genesis_engine/LegacyBackend/LegacyBackend.hpp"
#include "genesis_engine/LegacyRendering/LegacySceneRenderer.hpp"

#include "genesis_engine/scene/scene.hpp"
#include "genesis_engine/scene/entity.hpp"

namespace genesis_engine
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
		vec2u framebuffer_size = vec2u(0);

		Camera scene_camera;
		TransformD scene_camera_transform;

		//meters per second
		double linear_speed = 2.0;
		//rads per second
		double angular_speed = PI_D / 8.0;
		RenderSettings settings;
	};
}