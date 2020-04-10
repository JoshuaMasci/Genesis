#pragma once

#include <Genesis/Genesis.hpp>
#include <Genesis/Entity/World.hpp>
#include <Genesis/Rendering/BaseWorldRenderer.hpp>

#include <Genesis/LegacyBackend/LegacyBackend.hpp>

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

	Genesis::VertexBuffer vertex_buffer;
	Genesis::IndexBuffer index_buffer;
	uint32_t index_count;

	Genesis::ShaderProgram program;
};


