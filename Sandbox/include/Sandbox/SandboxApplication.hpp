#pragma once

#include <Genesis/Ecs/EcsWorld.hpp>
#include <Genesis/Entity/World.hpp>

#include "Genesis/Rendering/BaseImGui.hpp"

#include <Genesis/LegacyBackend/LegacyBackend.hpp>

#include <Genesis/Editor/ConsoleWindow.hpp>
#include <Genesis/Editor/HierarchyWindow.hpp>
#include <Genesis/Editor/EntityPropertiesWindow.hpp>

#include "Genesis/Physics/PhysicsSystem.hpp"
#include "Genesis/LegacyRendering/LegacyRenderingSystem.hpp"

#include "Genesis/Resource/GltfLoader.hpp"

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

	//Genesis::EcsWorld* ecs_world;
	//Genesis::PhyscisSystem* physics_system;
	//Genesis::LegacyRenderingSystem* rendering_system;

	Genesis::BaseImGui* ui_renderer;
	
	Genesis::vector2U offscreen_size;
	Genesis::Framebuffer offscreen_framebuffer;

	Genesis::ConsoleWindow console_window;
	Genesis::HierarchyWindow world_view_window;
	Genesis::EntityPropertiesWindow entity_properties_window;

	Genesis::GltfModel* model;
};


