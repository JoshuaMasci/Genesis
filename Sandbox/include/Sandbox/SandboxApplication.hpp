#pragma once

#include "Genesis/Rendering/BaseImGui.hpp"

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
	Genesis::BaseImGui* ui_renderer;
};


