#pragma once

#include <Genesis/Entity/World.hpp>
#include <Genesis/Rendering/BaseWorldRenderer.hpp>
#include "Genesis/Rendering/BaseImGui.hpp"

#include <Genesis/LegacyBackend/LegacyBackend.hpp>

#include <Genesis/Editor/ConsoleWindow.hpp>
#include <Genesis/Editor/HierarchyWindow.hpp>
#include <Genesis/Editor/EntityPropertiesWindow.hpp>
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
	Genesis::BaseWorldRenderer* world_renderer;
	Genesis::BaseImGui* ui_renderer;
	
	Genesis::vector2U offscreen_size;
	Genesis::Framebuffer offscreen_framebuffer;

	Genesis::ConsoleWindow console_window;
	Genesis::HierarchyWindow world_view_window;
	Genesis::EntityPropertiesWindow entity_properties_window;
};


