#pragma once

#include <Genesis/Genesis.hpp>

class SandboxApplication : public Genesis::GenesisApplication
{
public:
	virtual void run_tick(double delta_time) override;
};


