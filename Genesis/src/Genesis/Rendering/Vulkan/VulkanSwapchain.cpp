#include "VulkanSwapchain.hpp"

#include "Genesis/Rendering/Vulkan/VulkanQueueFamily.hpp"
#include <algorithm>

using namespace Genesis;

VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, VkExtent2D current_extent)
{
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
	{
		return capabilities.currentExtent;
	}
	else
	{
		VkExtent2D actualExtent = current_extent;
		actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
		actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

		return actualExtent;
	}
}

Genesis::VulkanSwapchain::VulkanSwapchain(VulkanDevice* device, VkExtent2D surface_size, VulkanSurface* surface)
{
	this->device = device->get();

	VkSurfaceCapabilitiesKHR capabilities;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device->getPhysicalDevice(), surface->get(), &capabilities);
	this->size = chooseSwapExtent(capabilities, surface_size);

	//Create Swapchain
	VkSwapchainCreateInfoKHR create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	create_info.surface = surface->get();

	create_info.minImageCount = surface->getImageCount();
	create_info.imageFormat = surface->getFormat().format;
	create_info.imageColorSpace = surface->getFormat().colorSpace;

	create_info.imageExtent = this->size;
	
	create_info.imageArrayLayers = 1;
	create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	uint32_t queueFamilyIndices[] = { device->getGraphicsFamilyIndex(), device->getPresentFamilyIndex() };

	if (queueFamilyIndices[0] != queueFamilyIndices[1])
	{
		create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		create_info.queueFamilyIndexCount = 2;
		create_info.pQueueFamilyIndices = queueFamilyIndices;
	}
	else
	{
		create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		create_info.queueFamilyIndexCount = 0;
		create_info.pQueueFamilyIndices = nullptr;
	}

	create_info.preTransform = surface->getCapabilities().currentTransform;
	create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	create_info.presentMode = surface->getPresentMode();
	create_info.clipped = VK_TRUE;
	create_info.oldSwapchain = VK_NULL_HANDLE;

	VkResult result = vkCreateSwapchainKHR(this->device, &create_info, nullptr, &this->swapchain);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create swap chain!");
	}

	vkGetSwapchainImagesKHR(device->get(), this->swapchain, &this->image_count, nullptr);
}

VulkanSwapchain::~VulkanSwapchain()
{
	vkDestroySwapchainKHR(this->device, this->swapchain, nullptr);
}