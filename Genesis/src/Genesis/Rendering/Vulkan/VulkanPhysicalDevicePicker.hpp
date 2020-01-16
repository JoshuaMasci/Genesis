#pragma once

#include "Genesis/Rendering/Vulkan/VulkanInclude.hpp"

namespace Genesis
{
	struct VulkanPhysicalDevicePicker
	{
		static VkPhysicalDevice pickDevice(VkInstance instance, VkSurfaceKHR surface);
	};
}