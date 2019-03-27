#pragma once

#include <Genesis/Genesis.hpp>

#include <Genesis/Graphics/Vulkan/VulkanRenderer.hpp>

class SandboxApplication : public Genesis::Application
{
public:
	SandboxApplication();
	virtual ~SandboxApplication();

	virtual void runFrame(double delta_time);
protected:
	Genesis::VulkanRenderer* renderer = nullptr;
};


