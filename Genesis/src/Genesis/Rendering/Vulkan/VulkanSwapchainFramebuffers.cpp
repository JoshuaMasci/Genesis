#include "VulkanSwapchainFramebuffers.hpp"

using namespace Genesis;

VulkanSwapchainFramebuffers::VulkanSwapchainFramebuffers(VulkanDevice* device, VulkanSwapchain* swapchain, VmaAllocator allocator, VkRenderPass screen_render_pass)
{
	this->device = device->getDevice();
	this->allocator = allocator;

	uint32_t swapchain_image_count = swapchain->getSwapchainImageCount();
	VkFormat swapchain_image_format = swapchain->getSwapchainFormat();
	VkFormat swapchain_depth_format = swapchain->getSwapchainDepthFormat();
	VkExtent2D swapchain_extent = swapchain->getSwapchainExtent();

	//Swapchain Images
	this->swapchain_images.resize(swapchain_image_count);
	vkGetSwapchainImagesKHR(device->getDevice(), swapchain->getSwapchain(), &swapchain_image_count, this->swapchain_images.data());

	//Swapchain Image Views
	this->swapchain_imageviews.resize(swapchain_image_count);
	for (size_t i = 0; i < this->swapchain_images.size(); i++)
	{
		VkImageViewCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		create_info.image = this->swapchain_images[i];
		create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
		create_info.format = swapchain_image_format;
		create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		create_info.subresourceRange.baseMipLevel = 0;
		create_info.subresourceRange.levelCount = 1;
		create_info.subresourceRange.baseArrayLayer = 0;
		create_info.subresourceRange.layerCount = 1;

		if (vkCreateImageView(device->getDevice(), &create_info, nullptr, &this->swapchain_imageviews[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create image views!");
		}
	}

	//Depth Images
	this->depth_images.resize(swapchain_image_count);
	this->depth_imageviews.resize(swapchain_image_count);
	this->depth_images_memory.resize(swapchain_image_count);
	for (size_t i = 0; i < swapchain_image_count; i++)
	{
		VkImageCreateInfo imageInfo = {};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = swapchain_extent.width;
		imageInfo.extent.height = swapchain_extent.height;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = 1;
		imageInfo.arrayLayers = 1;
		imageInfo.format = swapchain_depth_format;
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
		viewInfo.format = swapchain_depth_format;
		viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;

		if (vkCreateImageView(device->getDevice(), &viewInfo, nullptr, &this->depth_imageviews[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create texture image view!");
		}
	}

	//Swapchain Framebuffers
	this->swapchain_framebuffers.resize(swapchain_image_count);
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
		framebufferInfo.width = swapchain_extent.width;
		framebufferInfo.height = swapchain_extent.height;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(device->getDevice(), &framebufferInfo, nullptr, &this->swapchain_framebuffers[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create framebuffer!");
		}
	}

}

VulkanSwapchainFramebuffers::~VulkanSwapchainFramebuffers()
{
	for (size_t i = 0; i < this->swapchain_framebuffers.size(); i++)
	{
		vkDestroyFramebuffer(this->device, this->swapchain_framebuffers[i], nullptr);
		vkDestroyImageView(this->device, this->swapchain_imageviews[i], nullptr);
		vkDestroyImageView(this->device, this->depth_imageviews[i], nullptr);
		vmaDestroyImage(this->allocator, this->depth_images[i], this->depth_images_memory[i]);
	}
}
