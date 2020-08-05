#pragma once

#include "Genesis/Rendering/BaseImGui.hpp"

#include <Genesis/LegacyBackend/LegacyBackend.hpp>
#include <Genesis/LegacyRendering/LegacyWorldRenderer.hpp>
#include <Genesis/Editor/ConsoleWindow.hpp>

#include "Genesis_Editor/Windows/EntityListWindow.hpp"
#include "Genesis_Editor/Windows/EntityPropertiesWindow.hpp"
#include "Genesis_Editor/Windows/SceneWindow.hpp"

#include "Genesis/Resource/GltfLoader.hpp"

#include "Genesis/Ecs/Ecs.hpp"

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
		EntityListWindow* entity_list_window;
		EntityPropertiesWindow* entity_properties_window;
		SceneWindow* scene_window;

		GltfModel* scene_model = nullptr;

		EntityRegisty editor_registry;
	};
}

