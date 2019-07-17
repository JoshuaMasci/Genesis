#pragma once

#include "Genesis/Rendering/Vulkan/VulkanInclude.hpp"
#include "Genesis/Rendering/Vulkan/VulkanAllocator.hpp"
#include "Genesis/Rendering/Vulkan/VulkanCommandPool.hpp"

namespace Genesis
{
	class VulkanBuffer
	{
	public:
		VulkanBuffer(VulkanAllocator* allocator, uint64_t size_bytes, VkBufferUsageFlags type, VmaMemoryUsage memory_usage);
		~VulkanBuffer();

		void fillBuffer(VulkanCommandPool* transfer_pool, VkQueue transfer_queue, void* data, uint64_t data_size);

		inline VkBuffer get() { return this->buffer; };

	private:
		VulkanAllocator* allocator = nullptr;

		VkBuffer buffer;
		VmaAllocation buffer_memory;
		VmaAllocationInfo buffer_memory_info;
	};
}