#pragma once

#include "Genesis/Rendering/Vulkan/VulkanInclude.hpp"
#include "Genesis/Rendering/RenderingTypes.hpp"
#include "Genesis/Rendering/Vulkan/VulkanDevice.hpp"

#include "vk_mem_alloc.h"

namespace Genesis
{
	class VulkanAllocator
	{
	public:
		VulkanAllocator(VulkanDevice* device);
		~VulkanAllocator();

		bool isMemoryHostVisible(VmaAllocationInfo& memory_info);

		void mapMemory(VmaAllocation memory, void** data);
		void unmapMemory(VmaAllocation memory);

		void createBuffer(VkBufferCreateInfo* buffer_create, VmaMemoryUsage memory_usage, VkBuffer* buffer, VmaAllocation* memory, VmaAllocationInfo* memory_info);
		void destroyBuffer(VkBuffer buffer, VmaAllocation memory);

		void createImage(VkImageCreateInfo* image_create, VmaMemoryUsage memory_usage, VkImage* image, VmaAllocation* memory, VmaAllocationInfo* memory_info);
		void destroyImage(VkImage image, VmaAllocation memory);

	private:
		VmaAllocator allocator;
	};
}