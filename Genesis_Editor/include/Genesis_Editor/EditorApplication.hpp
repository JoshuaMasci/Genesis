#pragma once

#include "Genesis/Rendering/BaseImGui.hpp"

#include <Genesis/LegacyBackend/LegacyBackend.hpp>
#include <Genesis/LegacyRendering/LegacyWorldRenderer.hpp>
#include <Genesis/Editor/ConsoleWindow.hpp>

#include "Genesis_Editor/Windows/EntityHierarchyWindow.hpp"
#include "Genesis_Editor/Windows/EntityPropertiesWindow.hpp"
#include "Genesis_Editor/Windows/SceneWindow.hpp"
#include "Genesis_Editor/Windows/AssetBrowserWindow.hpp"

#include "Genesis/Ecs/Ecs.hpp"
#include "Genesis/Ecs/EntityWorld.hpp"

#include "Genesis/Resource/Material.hpp"
#include "Genesis/Resource/MeshPool.hpp"

#include "reactphysics3d.h"

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
		EntityHierarchyWindow* entity_hierarchy_window;
		EntityPropertiesWindow* entity_properties_window;
		SceneWindow* scene_window;
		AssetBrowserWindow* asset_browser_window;

		Material temp_material;

		//EntityRegistry editor_registry;
		//reactphysics3d::DynamicsWorld* editor_physics = nullptr;

		EntityWorld editor_world;

		MeshPool* mesh_pool = nullptr;
	};
}

