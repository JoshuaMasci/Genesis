#include "VulkanSwapChainSupportDetails.hpp"

using namespace Genesis;

VulkanSwapChainSupportDetails VulkanSwapChainSupportDetails::querySwapChainSupport(VkPhysicalDevice device, VulkanSurface* surface)
{
	VulkanSwapChainSupportDetails details;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface->getSurface(), &details.capabilities);

	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface->getSurface(), &formatCount, nullptr);

	if (formatCount != 0)
	{
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface->getSurface(), &formatCount, details.formats.data());
	}

	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface->getSurface(), &presentModeCount, nullptr);

	if (presentModeCount != 0)
	{
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface->getSurface(), &presentModeCount, details.presentModes.data());
	}

	return details;
}
