#pragma once

#include "Genesis/LegacyBackend/LegacyBackend.hpp"
#include "Genesis/Rendering/Camera.hpp"

#include "Genesis/Component/ModelComponent.hpp"

#include "Genesis/Rendering/Camera.hpp"
#include "Genesis/Rendering/Lights.hpp"

#include "Genesis/Rendering/SceneInfo.hpp"
#include "Genesis/Rendering/RenderSettings.hpp"

namespace Genesis
{
	class LegacySceneRenderer
	{
	public:
		LegacySceneRenderer(LegacyBackend* backend);
		~LegacySceneRenderer();

		void drawScene(vector2U target_size, Framebuffer target_framebuffer, SceneInfo& scene, RenderSettings& settings, CameraStruct& active_camera);

	protected:
		LegacyBackend* backend;

		ShaderProgram ambient_program;
		ShaderProgram directional_program;
		ShaderProgram point_program;
		//ShaderProgram spot_program;

		ShaderProgram gamma_correction_program;
	};
}