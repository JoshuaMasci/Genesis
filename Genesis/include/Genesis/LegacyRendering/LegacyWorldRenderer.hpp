#pragma once

#include "Genesis/LegacyBackend/LegacyBackend.hpp"
#include "Genesis/Rendering/Camera.hpp"
#include "Genesis/ECS/EntityRegistry.hpp"

namespace Genesis
{

	class LegacyWorldRenderer
	{
	public:
		LegacyWorldRenderer(LegacyBackend* backend);
		~LegacyWorldRenderer();

		void drawScene(vector2U target_size, Framebuffer target_framebuffer, EntityWorld* world, Camera& camera, TransformD& camera_transform);

	protected:
		LegacyBackend* backend;

		ShaderProgram ambient_program;
		ShaderProgram directional_program;
		//ShaderProgram pbr_point_program;
		//ShaderProgram pbr_spot_program;
	};
}