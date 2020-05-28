#pragma once

#include "Genesis/Ecs/EcsSystem.hpp"
#include "Genesis/LegacyBackend/LegacyBackend.hpp"

namespace Genesis
{
	class LegacyRenderingSystem : public RenderSystem
	{
	public:
		LegacyRenderingSystem(LegacyBackend* legacy_backend);
		~LegacyRenderingSystem();

		virtual void render(vector2U screen_size, EcsWorld* world, TimeStep time_step) override;

	protected:
		LegacyBackend* backend;

		ShaderProgram shader_program;
	};
}
