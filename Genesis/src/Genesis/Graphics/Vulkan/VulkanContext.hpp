#pragma once

#include "Genesis/Graphics/GraphicsContext.hpp"
#include "Genesis/Platform/Window.hpp"

namespace Genesis
{
	//Prototype
	class VulkanInstance;

	class VulkanContext : public GraphicsContext
	{
	public:
		VulkanContext(Window* window);
		~VulkanContext();

	private:
		VulkanInstance* vulkan = nullptr;
	};
}