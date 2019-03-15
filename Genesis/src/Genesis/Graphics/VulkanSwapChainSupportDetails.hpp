#pragma once

#include "Genesis/Graphics/VulkanDevice.hpp"
#include "Genesis/Graphics/VulkanSurface.hpp"

namespace Genesis
{
	class VulkanSwapChainSupportDetails
	{
	public:
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;

		static VulkanSwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VulkanSurface* surface);
	};
}