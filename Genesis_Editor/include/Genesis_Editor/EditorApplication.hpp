#pragma once

#include "Genesis/Rendering/BaseImGui.hpp"

#include "Genesis/LegacyBackend/LegacyBackend.hpp"
#include "Genesis/LegacyRendering/LegacySceneRenderer.hpp"

#include <memory>
#include "Genesis_Editor/Windows/EntityHierarchyWindow.hpp"
#include "Genesis_Editor/Windows/EntityPropertiesWindow.hpp"
#include "Genesis_Editor/Windows/SceneWindow.hpp"
#include "Genesis_Editor/Windows/AssetBrowserWindow.hpp"
#include "Genesis_Editor/Windows/MaterialEditorWindow.hpp"
#include "Genesis_Editor/Windows/RenderStatisticsWindow.hpp"

#include "Genesis/Resource/Material.hpp"
#include "Genesis/Resource/ResourceManager.hpp"

#include "reactphysics3d.h"

#include "Genesis/Scene/Ecs.hpp"
#include "Genesis/Scene/Scene.hpp"
#include "Genesis/Rendering/SceneRenderList.hpp"

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

