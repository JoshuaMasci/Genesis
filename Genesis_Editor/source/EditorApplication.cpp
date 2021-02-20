#include "Genesis_Editor/EditorApplication.hpp"

//Hardcoded for now
#include "SDL2_Platform.hpp" 
#include "SDL2_Window.hpp" 
#include "OpenglBackend.hpp"
#include "Genesis/LegacyRendering/LegacyImGui.hpp"
#include "Genesis/Platform/FileSystem.hpp"

#include "imgui.h"

#include "Genesis/Scene/SceneSerializer.hpp"

namespace Genesis
{
	EditorApplication::EditorApplication()
	{
		this->platform = new SDL2_Platform(this);
		this->window = new SDL2_Window(vector2U(1600, 900), "Genesis Editor");

		this->legacy_backend = new Opengl::OpenglBackend((SDL2_Window*)window);
		this->ui_renderer = new LegacyImGui(this->legacy_backend, this->input_manager, this->window);

		this->resource_manager = new ResourceManager(this->legacy_backend);

		this->entity_hierarchy_window = std::make_unique<EntityHierarchyWindow>(this->resource_manager);
		this->entity_properties_window = std::make_unique<EntityPropertiesWindow>(this->resource_manager);
		this->scene_window = std::make_unique<SceneWindow>(this->input_manager, this->legacy_backend);
		this->asset_browser_window = std::make_unique<AssetBrowserWindow>(this->legacy_backend, "res/");
		this->material_editor_window = std::make_unique<MaterialEditorWindow>(this->resource_manager);
		this->render_statistics_window = std::make_unique<RenderStatisticsWindow>(this->legacy_backend);

		this->editor_scene = new Scene();
	}

	EditorApplication::~EditorApplication()
	{
		this->console_window.release();
		this->entity_hierarchy_window.release();
		this->entity_properties_window.release();
		this->scene_window.release();
		this->asset_browser_window.release();
		this->material_editor_window.release();
		this->render_statistics_window.release();

		delete this->editor_scene;
		delete this->resource_manager;
		delete this->legacy_backend;
	}

	void EditorApplication::update(TimeStep time_step)
	{
		GENESIS_PROFILE_FUNCTION("EditorApplication::update");
		Application::update(time_step);
		this->scene_window->update(time_step);
	}

	void build_scene_render_list(Scene* scene);

	void EditorApplication::render(TimeStep time_step)
	{
		GENESIS_PROFILE_FUNCTION("EditorApplication::render");

		Application::render(time_step);

		this->legacy_backend->startFrame();
		vector4F clear_color = vector4F(0.0f, 0.0f, 0.0f, 1.0f);
		float clear_depth = 1.0f;
		this->legacy_backend->clearFramebuffer(true, true, &clear_color, &clear_depth);

		this->ui_renderer->beginFrame();
		this->ui_renderer->beginDocking();
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Open Scene", ""))
				{
					string save_file_path = FileSystem::openFileDialog("Supported Files(*.scene)\0*.scene;\0All files(*.*)\0*.*\0");

					if (!save_file_path.empty())
					{
						delete this->editor_scene;
						this->editor_scene = SceneSerializer().deserialize(save_file_path.c_str(), this->resource_manager);
					}
				}

				if (ImGui::MenuItem("Save Scene", ""))
				{
					string save_file_path = FileSystem::saveFileDialog("Supported Files(*.scene)\0*.scene;\0All files(*.*)\0*.*\0");

					if (!save_file_path.empty())
					{
						SceneSerializer().serialize(this->editor_scene, save_file_path.c_str());
					}
				}

				if (ImGui::MenuItem("Exit", "")) { this->close(); };
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Tools"))
			{
				ImGui::MenuItem("Imgui Demo", nullptr, &this->show_demo_window);
				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}
		this->ui_renderer->endDocking();

		this->render_statistics_window->draw(time_step);
		this->entity_hierarchy_window->draw(this->editor_scene);
		this->entity_properties_window->draw(this->entity_hierarchy_window->get_selected());
		this->asset_browser_window->draw();
		this->material_editor_window->draw();

		build_scene_render_list(this->editor_scene);
		this->scene_window->draw(this->editor_scene->render_list, this->editor_scene->lighting_settings, this->entity_hierarchy_window->get_selected());

		if (this->show_demo_window)
		{
			ImGui::ShowDemoWindow();
		}

		this->ui_renderer->endFrame();

		this->legacy_backend->endFrame();
	}


#include "Genesis/Component/ModelComponent.hpp"
#include "Genesis/Rendering/Camera.hpp"
#include "Genesis/Rendering/Lights.hpp"

	void add_to_render_list(SceneRenderList& render_list, EntityRegistry& registry, EntityHandle entity, const TransformD& parent_transform)
	{
		TransformD world_transform = parent_transform;

		if (registry.has<TransformD>(entity))
		{
			TransformUtils::transformByInplace(world_transform, parent_transform, registry.get<TransformD>(entity));
		}

		if (registry.has<ModelComponent>(entity))
		{
			ModelComponent& model = registry.get<ModelComponent>(entity);
			render_list.models.push_back({ model.mesh, model.material, world_transform });
		}

		if (registry.has<DirectionalLight>(entity))
		{
			DirectionalLight& light = registry.get<DirectionalLight>(entity);
			render_list.directional_lights.push_back({ light, world_transform });
		}

		if (registry.has<PointLight>(entity))
		{
			PointLight& light = registry.get<PointLight>(entity);
			render_list.point_lights.push_back({ light, world_transform });
		}

		if (registry.has<SpotLight>(entity))
		{
			SpotLight& light = registry.get<SpotLight>(entity);
			render_list.spot_lights.push_back({ light, world_transform });
		}

		for (EntityHandle child : EntityHiearchy(&registry, entity))
		{
			add_to_render_list(render_list, registry, child, world_transform);
		}
	}

	void build_scene_render_list(Scene* scene)
	{
		scene->render_list.clear();
		scene->registry.each([&](auto entity)
		{
			if (entity != scene->scene_components.handle())
			{
				if (!scene->registry.has<ChildNode>(entity))
				{
					add_to_render_list(scene->render_list, scene->registry, entity, TransformD());
				}
			}
		});
	}
}