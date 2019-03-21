#pragma once

#include "vulkan/vulkan.h"

namespace Genesis
{
	class VulkanPhysicalDevicePicker
	{
	public:
		static VkPhysicalDevice pickDevice(VkInstance instance, VkSurfaceKHR surface);
	};
}