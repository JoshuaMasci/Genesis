#pragma once

#include "vulkan_include.hpp"
#include "device.hpp"

namespace genesis
{
	class Image
	{
	protected:
		Device* device = nullptr;
		VkImage image = VK_NULL_HANDLE;
		VmaAllocation memory = VK_NULL_HANDLE;
		VkImageView view = VK_NULL_HANDLE;
		VkFormat format = VkFormat::VK_FORMAT_UNDEFINED;
		VkExtent2D size = {0, 0};

	public:
		Image(Device* device, const VkImageCreateInfo& create_info, VmaMemoryUsage memory_usage);
		~Image();

		inline VkImage get() const { return this->image; };
		inline VmaAllocation get_memory() const { return this->memory; };
		inline VkImageView get_view() const { return this->view; };
		inline VkFormat get_format() const { return this->format; };
		inline VkExtent2D get_size() const { return this->size; };
	};
}