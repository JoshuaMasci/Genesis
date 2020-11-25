#pragma once

#include "Genesis/Rendering/BaseImGui.hpp"

#include <Genesis/LegacyBackend/LegacyBackend.hpp>
#include <Genesis/LegacyRendering/LegacySceneRenderer.hpp>
#include <Genesis/Editor/ConsoleWindow.hpp>

#include "Genesis_Editor/Windows/EntityHierarchyWindow.hpp"
#include "Genesis_Editor/Windows/EntityPropertiesWindow.hpp"
#include "Genesis_Editor/Windows/SceneWindow.hpp"
#include "Genesis_Editor/Windows/AssetBrowserWindow.hpp"
#include "Genesis_Editor/Windows/MaterialEditorWindow.hpp"

#include "Genesis/Resource/Material.hpp"
#include "Genesis/Resource/MeshPool.hpp"
#include "Genesis/Resource/TexturePool.hpp"
#include "Genesis/Resource/MaterialPool.hpp"

#include "reactphysics3d.h"

#include "Genesis/Scene/Ecs.hpp"
#include "Genesis/Scene/Scene.hpp"
#include "Genesis/Rendering/SceneInfo.hpp"

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

		ConsoleWindow* console_window = nullptr;
		EntityHierarchyWindow* entity_hierarchy_window = nullptr;
		EntityPropertiesWindow* entity_properties_window = nullptr;
		SceneWindow* scene_window = nullptr;
		AssetBrowserWindow* asset_browser_window = nullptr;
		MaterialEditorWindow* material_editor_window = nullptr;

		MeshPool* mesh_pool = nullptr;
		TexturePool* texture_pool = nullptr;
		MaterialPool* material_pool = nullptr;

		bool show_demo_window = false;
	};
}

