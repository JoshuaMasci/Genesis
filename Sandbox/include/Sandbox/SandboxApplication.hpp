#pragma once

#include <Genesis/Entity/World.hpp>

#include "Genesis/Rendering/BaseImGui.hpp"

#include <Genesis/LegacyBackend/LegacyBackend.hpp>

#include <Genesis/Editor/ConsoleWindow.hpp>
#include <Genesis/Editor/WorldViewWindow.hpp>
#include <Genesis/Editor/EntityPropertiesWindow.hpp>

#include "Genesis/Resource/GltfLoader.hpp"
#include <Genesis/LegacyRendering/LegacyWorldRenderer.hpp>

class SandboxApplication : public Genesis::Application
{
public:
	SandboxApplication();
	virtual ~SandboxApplication();

	virtual void update(Genesis::TimeStep time_step) override;
	virtual void render(Genesis::TimeStep interpolation_value) override;
protected:
	Genesis::LegacyBackend* legacy_backend;

	Genesis::World* world;
	Genesis::World* secondary_world;

	Genesis::BaseImGui* ui_renderer;
	
	Genesis::vector2U offscreen_size;

	Genesis::ConsoleWindow console_window;
	Genesis::WorldViewWindow world_view_window;
	Genesis::EntityPropertiesWindow entity_properties_window;

	Genesis::LegacyWorldRenderer* world_renderer;

	Genesis::GltfModel* model;
	Genesis::GltfModel* cube_model;
	Genesis::GltfModel* inside_model;
};


