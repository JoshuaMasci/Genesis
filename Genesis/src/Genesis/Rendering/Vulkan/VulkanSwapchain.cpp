#include "VulkanSwapchain.hpp"

#include "Genesis/Rendering/Vulkan/VulkanQueueFamily.hpp"
#include <algorithm>

using namespace Genesis;

//Swapchain Utils
VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
	if (availableFormats.size() == 1 && availableFormats[0].format == VK_FORMAT_UNDEFINED)
	{
		return { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
	}

	for (const auto& availableFormat : availableFormats)
	{
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
		{
			return availableFormat;
		}
	}

	return availableFormats[0];
}

VkFormat findSupportedFormat(VkPhysicalDevice device, const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
{
	for (VkFormat format : candidates)
	{
		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties(device, format, &props);

		if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
		{
			return format;
		}
		else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
		{
			return format;
		}
	}

	throw std::runtime_error("failed to find supported format!");
}

VkFormat findDepthFormat(VkPhysicalDevice device)
{
	return findSupportedFormat(device, { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT }, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes)
{
	VkPresentModeKHR bestMode = VK_PRESENT_MODE_FIFO_KHR;

	for (const auto& availablePresentMode : availablePresentModes)
	{
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
		{
			return availablePresentMode;
		}
		else if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR)
		{
			bestMode = availablePresentMode;
		}
	}

	return bestMode;
}

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

VulkanSwapchain::VulkanSwapchain(VulkanDevice* vulkan_device, Window* window, VkSurfaceKHR surface)
{
	this->device = vulkan_device->getDevice();

	VulkanSwapChainSupportDetails swapChainSupport = VulkanSwapChainSupportDetails::querySwapChainSupport(vulkan_device->getPhysicalDevice(), surface);
	VulkanQueueFamilyAllocator queue_allocator(vulkan_device->getPhysicalDevice(), surface);

	//Choose Formats
	VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
	VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
	vector2U window_size = window->getWindowSize();
	VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities, { window_size.x, window_size.y });

	//Choose Image Count
	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
	if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
	{
		imageCount = swapChainSupport.capabilities.maxImageCount;
	}

	//Create Swapchain
	VkSwapchainCreateInfoKHR create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	create_info.surface = surface;

	create_info.minImageCount = imageCount;
	create_info.imageFormat = surfaceFormat.format;
	create_info.imageColorSpace = surfaceFormat.colorSpace;
	create_info.imageExtent = extent;
	create_info.imageArrayLayers = 1;
	create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	uint32_t queueFamilyIndices[] = { queue_allocator.graphics.queue_family, queue_allocator.present.queue_family };

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

	create_info.preTransform = swapChainSupport.capabilities.currentTransform;
	create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	create_info.presentMode = presentMode;
	create_info.clipped = VK_TRUE;
	create_info.oldSwapchain = VK_NULL_HANDLE;

	VkResult result = vkCreateSwapchainKHR(vulkan_device->getDevice(), &create_info, nullptr, &this->swapchain);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create swap chain!");
	}

	//Swapchain Images
	vkGetSwapchainImagesKHR(vulkan_device->getDevice(), this->swapchain, &imageCount, nullptr);
	this->swapchain_images.resize(imageCount);
	vkGetSwapchainImagesKHR(vulkan_device->getDevice(), this->swapchain, &imageCount, this->swapchain_images.data());

	this->swapchain_image_count = imageCount;
	this->swapchain_image_format = surfaceFormat.format;
	this->swapchain_extent = extent;

	//Swapchain Image Views
	this->swapchain_imageviews.resize(this->swapchain_image_count);
	for (size_t i = 0; i < this->swapchain_images.size(); i++)
	{
		VkImageViewCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		create_info.image = this->swapchain_images[i];
		create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
		create_info.format = this->swapchain_image_format;
		create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		create_info.subresourceRange.baseMipLevel = 0;
		create_info.subresourceRange.levelCount = 1;
		create_info.subresourceRange.baseArrayLayer = 0;
		create_info.subresourceRange.layerCount = 1;

		if (vkCreateImageView(vulkan_device->getDevice(), &create_info, nullptr, &this->swapchain_imageviews[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create image views!");
		}
	}
}


VulkanSwapchain::~VulkanSwapchain()
{
	for (auto framebuffer : this->swapchain_framebuffers)
	{
		vkDestroyFramebuffer(this->device, framebuffer, nullptr);
	}

	for (auto imageView : this->swapchain_imageviews)
	{
		vkDestroyImageView(this->device, imageView, nullptr);
	}

	vkDestroySwapchainKHR(this->device, this->swapchain, nullptr);
}

void VulkanSwapchain::buildSwapchainFramebuffers(VkRenderPass screen_render_pass)
{
	//Swapchain Framebuffers
	this->swapchain_framebuffers.resize(this->swapchain_image_count);

	for (size_t i = 0; i < this->swapchain_framebuffers.size(); i++)
	{
		Array<VkImageView> attachments(2);
		attachments[0] = this->swapchain_imageviews[i];
		attachments[1] = this->depth_imageviews[i];

		VkFramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = screen_render_pass;
		framebufferInfo.attachmentCount = (uint32_t)attachments.size();
		framebufferInfo.pAttachments = attachments.data();
		framebufferInfo.width = this->swapchain_extent.width;
		framebufferInfo.height = this->swapchain_extent.height;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(this->device, &framebufferInfo, nullptr, &this->swapchain_framebuffers[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create framebuffer!");
		}
	}
}

void VulkanSwapchain::buildSwapchainDepthBuffers(VkFormat depth_format, VmaAllocator allocator)
{
	this->depth_images.resize(this->swapchain_image_count);
	this->depth_imageviews.resize(this->swapchain_image_count);
	this->depth_images_memory.resize(this->swapchain_image_count);

	for (size_t i = 0; i < this->swapchain_image_count; i++)
	{
		VkImageCreateInfo imageInfo = {};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = this->swapchain_extent.width;
		imageInfo.extent.height = this->swapchain_extent.height;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = 1;
		imageInfo.arrayLayers = 1;
		imageInfo.format = depth_format;
		imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VmaAllocationCreateInfo depthImageAllocCreateInfo = {};
		depthImageAllocCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
		vmaCreateImage(allocator, &imageInfo, &depthImageAllocCreateInfo, &this->depth_images[i], &this->depth_images_memory[i], nullptr);

		VkImageViewCreateInfo viewInfo = {};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = this->depth_images[i];
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = depth_format;
		viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;

		if (vkCreateImageView(device, &viewInfo, nullptr, &this->depth_imageviews[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create texture image view!");
		}
	}
}

VulkanSwapChainSupportDetails VulkanSwapChainSupportDetails::querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface)
{
	VulkanSwapChainSupportDetails details;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
	if (formatCount != 0)
	{
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
	}

	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);
	if (presentModeCount != 0)
	{
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
	}

	return details;
}