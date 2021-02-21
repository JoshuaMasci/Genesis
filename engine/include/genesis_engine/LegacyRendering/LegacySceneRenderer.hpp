#pragma once

#include "genesis_engine/LegacyBackend/LegacyBackend.hpp"
#include "genesis_engine/rendering/camera.hpp"

#include "genesis_engine/component/model_component.hpp"

#include "genesis_engine/rendering/camera.hpp"
#include "genesis_engine/rendering/lights.hpp"

#include "genesis_engine/rendering/scene_lighting_settings.hpp"
#include "genesis_engine/rendering/scene_render_list.hpp"
#include "genesis_engine/rendering/render_settings.hpp"

namespace genesis_engine
{
	class LegacySceneRenderer
	{
	public:
		LegacySceneRenderer(LegacyBackend* backend);
		~LegacySceneRenderer();

		void draw_scene(vec2u target_size, Framebuffer target_framebuffer, SceneRenderList& scene, SceneLightingSettings& lighting, RenderSettings& settings, CameraStruct& active_camera);

	protected:
		LegacyBackend* backend;

		ShaderProgram ambient_program;
		ShaderProgram directional_program;
		ShaderProgram point_program;
		//ShaderProgram spot_program;

		ShaderProgram gamma_correction_program;
	};
}