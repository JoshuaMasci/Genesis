#pragma once

#include <Genesis/Genesis.hpp>
#include <Genesis/Entity/World.hpp>
#include <Genesis/Entity/WorldRenderer.hpp>
#include <Genesis/Scene/MeshPool.hpp>

class SandboxApplication : public Genesis::Application
{
public:
	SandboxApplication();
	virtual ~SandboxApplication();

	virtual void update(Genesis::TimeStep time_step) override;
	virtual void render(Genesis::TimeStep interpolation_value) override;
protected:

	Genesis::World* world = nullptr;
	Genesis::WorldRenderer* world_renderer = nullptr;
	Genesis::MeshPool* mesh_pool = nullptr;
};


