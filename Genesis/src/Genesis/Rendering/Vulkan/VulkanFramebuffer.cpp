#include "VulkanFramebuffer.hpp"

using namespace Genesis;

VulkanFramebuffer::VulkanFramebuffer(VulkanDevice* device, VkExtent2D size, List<VkFormat>& color_formats, VkFormat depth_format, VkRenderPass render_pass)
{
	this->device = device;
	this->size = size;
	this->render_pass = render_pass;

	vector<VkImageView> image_views;

	this->images.resize(color_formats.size());
	for (uint16_t i = 0; i < this->images.size(); i++)
	{
		VkImageCreateInfo image_info = {};
		image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		image_info.imageType = VK_IMAGE_TYPE_2D;
		image_info.extent.width = this->size.width;
		image_info.extent.height = this->size.height;
		image_info.extent.depth = 1;
		image_info.mipLevels = 1;
		image_info.arrayLayers = 1;
		image_info.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		image_info.format = color_formats[i];
		image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
		image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		image_info.samples = VK_SAMPLE_COUNT_1_BIT;
		image_info.flags = 0;

		this->device->createImage(&image_info, VMA_MEMORY_USAGE_GPU_ONLY, &this->images[i].image, &this->images[i].image_memory, nullptr);
	
		VkImageViewCreateInfo view_info = {};
		view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		view_info.image =	this->images[i].image;
		view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
		view_info.format = color_formats[i];
		view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		view_info.subresourceRange.baseMipLevel = 0;
		view_info.subresourceRange.levelCount = 1;
		view_info.subresourceRange.baseArrayLayer = 0;
		view_info.subresourceRange.layerCount = 1;

		if (vkCreateImageView(this->device->get(), &view_info, nullptr, &this->images[i].image_view) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create color image view!");
		}

		this->images[i].image_format = color_formats[i];
		image_views.push_back(this->images[i].image_view);
	}

	if (depth_format != VK_FORMAT_UNDEFINED)
	{
		VkImageCreateInfo image_info = {};
		image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		image_info.imageType = VK_IMAGE_TYPE_2D;
		image_info.extent.width = this->size.width;
		image_info.extent.height = this->size.height;
		image_info.extent.depth = 1;
		image_info.mipLevels = 1;
		image_info.arrayLayers = 1;
		image_info.format = depth_format;
		image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
		image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		image_info.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		image_info.samples = VK_SAMPLE_COUNT_1_BIT;
		image_info.flags = 0;

		this->device->createImage(&image_info, VMA_MEMORY_USAGE_GPU_ONLY, &this->depth_image.image, &this->depth_image.image_memory, nullptr);

		VkImageViewCreateInfo view_info = {};
		view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		view_info.image = this->depth_image.image;
		view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
		view_info.format = depth_format;
		view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		view_info.subresourceRange.baseMipLevel = 0;
		view_info.subresourceRange.levelCount = 1;
		view_info.subresourceRange.baseArrayLayer = 0;
		view_info.subresourceRange.layerCount = 1;

		if (vkCreateImageView(this->device->get(), &view_info, nullptr, &this->depth_image.image_view) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create depth image view!");
		}

		this->depth_image.image_format = depth_format;
		image_views.push_back(this->depth_image.image_view);
	}

	VkFramebufferCreateInfo framebuffer_info = {};
	framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	framebuffer_info.renderPass = this->render_pass;

	framebuffer_info.attachmentCount = (uint32_t) image_views.size();
	framebuffer_info.pAttachments = image_views.data();

	framebuffer_info.width = this->size.width;
	framebuffer_info.height = this->size.height;
	framebuffer_info.layers = 1;

	if (vkCreateFramebuffer(this->device->get(), &framebuffer_info, nullptr, &this->framebuffer) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create frameBuffer!");
	}

	this->clear_values.resize(color_formats.size());
	for (size_t i = 0; i < clear_values.size(); i++)
	{
		this->clear_values[i].color = { 0.f, 0.0f, 0.0f, 0.0f };
	}

	if (depth_format != VK_FORMAT_UNDEFINED)
	{
		size_t array_size = clear_values.size();
		this->clear_values.resize(array_size + 1);
		this->clear_values[array_size].depthStencil = { 1.0f, 0 };
	}
}

VulkanFramebuffer::~VulkanFramebuffer()
{
	vkDestroyFramebuffer(this->device->get(), this->framebuffer, nullptr);

	if (this->depth_image.image_format != VK_FORMAT_UNDEFINED)
	{
		vkDestroyImageView(this->device->get(), this->depth_image.image_view, nullptr);
		this->device->destroyImage(this->depth_image.image, this->depth_image.image_memory);
	}

	for (size_t i = 0; i < this->images.size(); i++)
	{
		vkDestroyImageView(this->device->get(), this->images[i].image_view, nullptr);
		this->device->destroyImage(this->images[i].image, this->images[i].image_memory);
	}
}

VulkanFramebufferSet::VulkanFramebufferSet(VulkanDevice* device, VkExtent2D size, List<VkFormat>& color_formats, VkFormat depth_format, VkRenderPass render_pass, uint32_t frame_count)
{
	this->device = device;
	this->size = size;
	this->color_formats = color_formats;
	this->depth_format = depth_format;
	this->render_pass = render_pass;

	this->framebuffers.resize(frame_count);
	for (size_t i = 0; i < this->framebuffers.size(); i++)
	{
		this->framebuffers[i] = this->buildFramebuffer();
	}
}

VulkanFramebufferSet::~VulkanFramebufferSet()
{
	for (size_t i = 0; i < this->framebuffers.size(); i++)
	{
		delete this->framebuffers[i];
	}
}
