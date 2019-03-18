#pragma once

#include <Genesis/Genesis.hpp>

#include <Genesis/Graphics/VulkanGraphics.hpp>

class SandboxApplication : public Genesis::Application
{
public:
	SandboxApplication();
	virtual ~SandboxApplication();

	virtual void runFrame(double delta_time);
protected:
	Genesis::VulkanGraphics* graphics;
};


