#pragma once

#include "Genesis/Ecs/Ecs.hpp"

#include "Genesis/LegacyBackend/LegacyBackend.hpp"
#include "Genesis/Rendering/Camera.hpp"

namespace Genesis
{

	class LegacyWorldRenderer
	{
	public:
		LegacyWorldRenderer(LegacyBackend* backend);
		~LegacyWorldRenderer();

		void drawScene(vector2U target_size, Framebuffer target_framebuffer, EntityRegisty& world, Camera& camera, TransformD& camera_transform);

	protected:
		LegacyBackend* backend;

		ShaderProgram ambient_program;
		ShaderProgram directional_program;
		//ShaderProgram point_program;
		//ShaderProgram spot_program;

		ShaderProgram gamma_correction_program;
	};
}