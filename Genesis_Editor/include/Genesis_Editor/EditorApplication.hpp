#pragma once

#include "Genesis/Rendering/BaseImGui.hpp"

#include <Genesis/LegacyBackend/LegacyBackend.hpp>
#include <Genesis/LegacyRendering/LegacyWorldRenderer.hpp>
#include <Genesis/ECS/EntityRegistry.hpp>
#include <Genesis/Editor/ConsoleWindow.hpp>

#include "Genesis_Editor/Windows/WorldViewWindow.hpp"
#include "Genesis_Editor/Windows/EntityPropertiesWindow.hpp"
#include "Genesis_Editor/Windows/SceneViewWindow.hpp"

namespace Genesis
{
	class EditorApplication : public Application
	{
	public:
		EditorApplication();
		virtual ~EditorApplication();

		virtual void update(TimeStep time_step) override;
		virtual void render(TimeStep interpolation_value) override;
	protected:
		LegacyBackend* legacy_backend;

		BaseImGui* ui_renderer;

		vector2U offscreen_size;

		ConsoleWindow* console_window;
		WorldViewWindow* world_view_window;
		EntityPropertiesWindow* entity_properties_window;
		SceneViewWindow* scene_view_window;

		EntityRegistry* editor_registry;
		EntityWorld* editor_base_world;
	};
}

