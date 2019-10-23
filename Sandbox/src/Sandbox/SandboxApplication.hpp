#pragma once

#include <Genesis/Genesis.hpp>

class SandboxApplication : public Genesis::Application
{
public:
	SandboxApplication();
	virtual ~SandboxApplication();

	virtual void drawWorld(double delta_time);
protected:
};


