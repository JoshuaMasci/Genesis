#pragma once

#include <Genesis/Genesis.hpp>

class SandboxApplication : public Genesis::Application
{
public:
	SandboxApplication();
	virtual ~SandboxApplication();

	virtual void update(double delta_time) override;
	virtual void render(double delta_time) override;
protected:
};


