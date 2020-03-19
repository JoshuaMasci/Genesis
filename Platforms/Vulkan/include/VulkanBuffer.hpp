#pragma once

#include "VulkanInclude.hpp"
#include "VulkanDevice.hpp"

#include "Genesis/RenderingBackend/RenderingTypes.hpp"
#include "Genesis/RenderingBackend/VertexInputDescription.hpp"

namespace Genesis
{
	class VulkanBuffer
	{
	public:
		VulkanBuffer() {};
		VulkanBuffer(VulkanDevice* device, uint64_t size_bytes, VkBufferUsageFlags buffer_usage, VmaMemoryUsage memory_usage);
		virtual ~VulkanBuffer();

		void fillBuffer(void* data, uint64_t data_size);

		inline VkBuffer get() { return this->buffer; };
		inline uint64_t getSize() { return this->size; };
		inline bool isHostVisable() { return this->host_visable; };

	protected:
		VulkanDevice* device = nullptr;

		VkBuffer buffer = VK_NULL_HANDLE;
		VmaAllocation buffer_memory = VK_NULL_HANDLE;

		uint64_t size = 0;
		bool host_visable = false;
	};

	class VulkanDynamicBuffer
	{
	public:
		VulkanDynamicBuffer(VulkanDevice* device, uint64_t data_size, VkBufferUsageFlags buffer_usage, VmaMemoryUsage memory_usage, uint32_t frame_count);
		~VulkanDynamicBuffer();

		void incrementIndex();

		inline VulkanBuffer* getCurrentBuffer() { return this->buffers[this->current_index]; };
		inline uint64_t getSize() { return this->size; };

	protected:
		List<VulkanBuffer*> buffers;
		uint32_t current_index;
		uint64_t size = 0;
		bool has_changed;
	};
}