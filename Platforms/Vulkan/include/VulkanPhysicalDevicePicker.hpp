#pragma once

#include "VulkanInclude.hpp"

namespace Genesis
{
	struct VulkanPhysicalDevicePicker
	{
		static VkPhysicalDevice pickDevice(VkInstance instance, VkSurfaceKHR surface);
	};
}