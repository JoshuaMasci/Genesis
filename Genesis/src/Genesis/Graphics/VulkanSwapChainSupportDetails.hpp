#pragma once

#include "Genesis/Core/Types.hpp"
#include <vulkan/vulkan.h>

namespace Genesis
{
	class VulkanSwapChainSupportDetails
	{
	public:
		VkSurfaceCapabilitiesKHR capabilities;
		vector<VkSurfaceFormatKHR> formats;
		vector<VkPresentModeKHR> presentModes;

		static VulkanSwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);
	};
}