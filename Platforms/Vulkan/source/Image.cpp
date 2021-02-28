#include "vulkan_renderer/image.hpp"

namespace genesis
{
	//TODO allow 1d, 3d, cube images 
	Image::Image(Device* device, const VkImageCreateInfo& create_info, VmaMemoryUsage memory_usage)
	{
		this->device = device;
		this->format = create_info.format;
		this->size = { create_info.extent.width, create_info.extent.height };

		VmaAllocationInfo allocation_info;
		this->device->create_image(&create_info, memory_usage, &this->image, &this->memory, &allocation_info);

		VkImageViewCreateInfo view_info = {};
		view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		view_info.image = this->image;
		view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
		view_info.format = create_info.format;
		view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		view_info.subresourceRange.baseMipLevel = 0;
		view_info.subresourceRange.levelCount = create_info.mipLevels;
		view_info.subresourceRange.baseArrayLayer = 0;
		view_info.subresourceRange.layerCount = create_info.arrayLayers;
		VK_ASSERT(vkCreateImageView(this->device->get(), &view_info, nullptr, &this->view));
	}

	Image::~Image()
	{
		if (this->device != nullptr)
		{
			vkDestroyImageView(this->device->get(), this->view, nullptr);
			this->device->destroy_image(this->image, this->memory);
		}
	}
}