#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include "Genesis/Graphics/VulkanInstance.hpp"
#include "Genesis/Platform/Window.hpp"
namespace Genesis
{
	class VulkanSurface
	{
	public:
		VulkanSurface(VulkanInstance* instance, Window* window);
		~VulkanSurface();

		VkSurfaceKHR getSurface();

	private:
		VulkanInstance* instance;
		VkSurfaceKHR surface;
	};
}