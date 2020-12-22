#pragma once

#include "Genesis/Rendering/BaseImGui.hpp"
#include "Genesis/LegacyBackend/LegacyBackend.hpp"
#include "Genesis/LegacyRendering/LegacySceneRenderer.hpp"
#include "Genesis/Scene/Scene.hpp"
#include "Genesis/Resource/ResourceManager.hpp"

namespace Genesis
{
	class SandboxApplication : public Genesis::Application
	{
	public:
		SandboxApplication();
		virtual ~SandboxApplication();

		virtual void update(Genesis::TimeStep time_step) override;
		virtual void render(Genesis::TimeStep interpolation_value) override;
	protected:
		Scene* sandbox_scene = nullptr;

		LegacyBackend* legacy_backend;
		BaseImGui* ui_renderer;
		ResourceManager* resource_manager = nullptr;

		LegacySceneRenderer* world_renderer = nullptr;
		Framebuffer framebuffer = nullptr;
		vector2U framebuffer_size = vector2U(0, 0);

		Camera scene_camera;
		TransformD scene_camera_transform;
		RenderSettings settings;
	};
}


