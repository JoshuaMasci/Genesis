#pragma once

#include "genesis_engine/rendering/base_imgui.hpp"

#include "genesis_engine/LegacyBackend/LegacyBackend.hpp"
#include "genesis_engine/LegacyRendering/LegacySceneRenderer.hpp"

#include <memory>
#include "genesis_editor/panels/entity_hierarchy_panel.hpp"
#include "genesis_editor/panels/entity_properties_panel.hpp"
#include "genesis_editor/panels/scene_panel.hpp"
#include "genesis_editor/panels/asset_browser_panel.hpp"
#include "genesis_editor/panels/material_editor_panel.hpp"
#include "genesis_editor/panels/render_statistics_panel.hpp"

#include "genesis_engine/resource/material.hpp"
#include "genesis_engine/resource/resource_manager.hpp"

#include "genesis_engine/scene/ecs.hpp"
#include "genesis_engine/scene/Scene.hpp"
#include "genesis_engine/rendering/scene_render_list.hpp"

namespace genesis_engine
{
	class EditorApplication : public Application
	{
	public:
		EditorApplication();
		virtual ~EditorApplication();

		virtual void update(TimeStep time_step) override;
		virtual void render(TimeStep interpolation_value) override;
	protected:
		Scene* editor_scene = nullptr;

		LegacyBackend* legacy_backend;
		BaseImGui* ui_renderer;
		ResourceManager* resource_manager = nullptr;

		std::unique_ptr<ConsoleWindow> console_window;
		std::unique_ptr<EntityHierarchyWindow> entity_hierarchy_window;
		std::unique_ptr<EntityPropertiesWindow> entity_properties_window;
		std::unique_ptr<SceneWindow> scene_window;
		std::unique_ptr<AssetBrowserWindow> asset_browser_window;
		std::unique_ptr<MaterialEditorWindow> material_editor_window;
		std::unique_ptr<RenderStatisticsWindow> render_statistics_window;
		bool show_demo_window = false;
	};
}

