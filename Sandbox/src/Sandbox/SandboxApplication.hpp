#pragma once

#include <Genesis/Genesis.hpp>
#include <Genesis/Entity/World.hpp>

class SandboxApplication : public Genesis::Application
{
public:
	SandboxApplication();
	virtual ~SandboxApplication();

	virtual void update(Genesis::TimeStep time_step) override;
	virtual void render(Genesis::TimeStep time_step) override;
protected:

	Genesis::World* world = nullptr;
};


