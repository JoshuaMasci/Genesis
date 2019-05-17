#pragma once

#include "Genesis/Rendering/Vulkan/VulkanInclude.hpp"

namespace Genesis
{
	class VulkanPhysicalDevicePicker
	{
	public:
		static VkPhysicalDevice pickDevice(VkInstance instance, VkSurfaceKHR surface);
	};
}