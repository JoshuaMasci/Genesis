#pragma once

#include <Genesis/Genesis.hpp>

#include <Genesis/Graphics/VulkanGraphics.hpp>

class SandboxApplication : public Genesis::Application
{
public:
	SandboxApplication();
	virtual ~SandboxApplication();

protected:
	Genesis::VulkanGraphics* graphics;
};


