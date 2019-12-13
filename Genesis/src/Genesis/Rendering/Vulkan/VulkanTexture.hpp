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

		void fillTexture(VulkanCommandPool* transfer_pool, VkQueue transfer_queue, void* data, uint64_t data_size);

		inline VkImage get() { return this->image; };
		inline VkImageView getImageView() { return this->image_view; };

	private:
		VulkanDevice* device;

		VkExtent2D size;
		VkImage image;
		VmaAllocation image_memory;
		VmaAllocationInfo image_memory_info;
		VkImageView image_view;
	};
}