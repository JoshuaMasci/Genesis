#pragma once

#include <Genesis/Genesis.hpp>
#include <Genesis/Entity/World.hpp>
//#include <Genesis/Entity/WorldRenderer.hpp>
//#include <Genesis/Resource/MeshPool.hpp>
//#include <Genesis/Resource/MaterialPool.hpp>

#include <Genesis/LegacyBackend/LegacyBackend.hpp>

class SandboxApplication : public Genesis::Application
{
public:
	SandboxApplication();
	virtual ~SandboxApplication();

	virtual void update(Genesis::TimeStep time_step) override;
	virtual void render(Genesis::TimeStep interpolation_value) override;
protected:

	Genesis::World* world = nullptr;

	Genesis::LegacyBackend* legacy_backend;

	//Genesis::WorldRenderer* world_renderer = nullptr;
	//Genesis::MeshPool* mesh_pool = nullptr;
	//Genesis::MaterialPool* material_pool = nullptr;

	Genesis::Buffer vertex_buffer;
	Genesis::Buffer index_buffer;
	uint32_t index_count;

	Genesis::ShaderProgram program;
};


