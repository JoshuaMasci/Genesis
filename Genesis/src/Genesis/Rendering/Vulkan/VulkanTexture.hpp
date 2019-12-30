#pragma once

#include "Genesis/Rendering/Vulkan/VulkanInclude.hpp"
#include "Genesis/Rendering/Vulkan/VulkanDevice.hpp"
#include "Genesis/Rendering/Vulkan/VulkanCommandPool.hpp"

namespace Genesis
{
	class VulkanTexture
	{
	public:
		VulkanTexture(VulkanDevice* device, VkExtent2D size, VmaMemoryUsage memory_usage);
		~VulkanTexture();

		inline VkImage get() { return this->image; };
		inline VkImageView getImageView() { return this->image_view; };

		inline VkExtent2D getSize() { return this->size; };
		inline VkFormat getFormat() { return this->format; };
		inline VkImageLayout getInitialLayout() { return this->initial_layout; };

	private:
		VulkanDevice* device;

		VkExtent2D size;
		VkFormat format;
		VkImageLayout initial_layout;

		VkImage image;
		VmaAllocation image_memory;
		VmaAllocationInfo image_memory_info;
		VkImageView image_view;
	};
}