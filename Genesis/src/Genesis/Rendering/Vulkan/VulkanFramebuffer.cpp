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

	VkFramebufferCreateInfo framebufferInfo = {};
	framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	framebufferInfo.renderPass = this->render_pass;

	framebufferInfo.attachmentCount = (uint32_t) image_views.size();
	framebufferInfo.pAttachments = image_views.data();

	framebufferInfo.width = this->size.width;
	framebufferInfo.height = this->size.height;
	framebufferInfo.layers = 1;

	if (vkCreateFramebuffer(this->device->get(), &framebufferInfo, nullptr, &this->framebuffer) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create framebuffer!");
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

/*VulkanFramebufferSet::VulkanFramebufferSet(VulkanDevice* device, VkExtent2D size, List<VkFormat>& color_formats, VkFormat depth_format, VkRenderPass render_pass, uint32_t frame_count)
{
	this->device = device;
	this->size = size;
	this->color_formats = color_formats;
	this->depth_format = depth_format;
	this->render_pass = render_pass;

	this->frame_buffers.resize(frame_count);
	for (size_t i = 0; i < this->frame_buffers.size(); i++)
	{
		this->frame_buffers[i] = new VulkanFramebuffer(device, size, color_formats, depth_format, render_pass);
	}
}

VulkanFramebufferSet::~VulkanFramebufferSet()
{
	for (size_t i = 0; i < this->frame_buffers.size(); i++)
	{
		delete this->frame_buffers[i];
	}
}

void VulkanFramebufferSet::startFrame(uint32_t frame)
{
	VkExtent2D current_size = this->frame_buffers[frame]->getSize();
	if (current_size.width != this->size.width || current_size.height != this->size.height)
	{
		delete this->frame_buffers[frame];
		this->frame_buffers[frame] = new VulkanFramebuffer(this->device, this->size, this->color_formats, this->depth_format, this->render_pass);
	}
}*/
