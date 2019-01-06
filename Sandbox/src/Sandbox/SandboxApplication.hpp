#pragma once

#include <Genesis/Genesis.hpp>

class SandboxApplication : public Genesis::Application
{
public:
	virtual void run_tick(double delta_time) override;
};


