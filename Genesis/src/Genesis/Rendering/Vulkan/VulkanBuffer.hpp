#pragma once

#include "Genesis/Rendering/Buffer.hpp"
#include "Genesis/Rendering/Vulkan/VulkanInclude.hpp"

namespace Genesis
{
	class VulkanBuffer : public Buffer
	{
	public:
		VulkanBuffer(VmaAllocator allocator, uint32_t size_bytes, VkBufferUsageFlags usage, VmaMemoryUsage memory_usage);
		virtual ~VulkanBuffer();

		virtual void fillBuffer(void* data, uint32_t data_size);
	private:
		VmaAllocator allocator;
		uint32_t size;
		VkBuffer buffer;
		VmaAllocation buffer_memory;
		VmaAllocationInfo buffer_memory_info;
		// Inherited via Buffer
	};
}