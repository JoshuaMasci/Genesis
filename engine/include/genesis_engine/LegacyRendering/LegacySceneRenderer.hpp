#pragma once

#include "genesis_engine/LegacyBackend/LegacyBackend.hpp"
#include "genesis_engine/rendering/Camera.hpp"

#include "genesis_engine/component/ModelComponent.hpp"

#include "genesis_engine/rendering/Camera.hpp"
#include "genesis_engine/rendering/Lights.hpp"

#include "genesis_engine/rendering/SceneLightingSettings.hpp"
#include "genesis_engine/rendering/SceneRenderList.hpp"
#include "genesis_engine/rendering/RenderSettings.hpp"

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