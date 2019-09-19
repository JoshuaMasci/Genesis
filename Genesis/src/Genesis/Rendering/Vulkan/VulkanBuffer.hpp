#pragma once

#include "Genesis/Rendering/Vulkan/VulkanInclude.hpp"
#include "Genesis/Rendering/Vulkan/VulkanAllocator.hpp"
#include "Genesis/Rendering/Vulkan/VulkanCommandPool.hpp"
#include "Genesis/Rendering/Vulkan/VulkanDescriptorPool.hpp"

#include "Genesis/Rendering/VertexInputDescription.hpp"

namespace Genesis
{
	class VulkanBuffer
	{
	public:
		VulkanBuffer(VulkanAllocator* allocator, uint64_t size_bytes, VkBufferUsageFlags type, VmaMemoryUsage memory_usage);
		virtual ~VulkanBuffer();

		void fillBuffer(VulkanCommandPool* transfer_pool, VkQueue transfer_queue, void* data, uint64_t data_size);
		void fillBufferHostVisable(void* data, uint64_t data_size);

		inline VkBuffer get() { return this->buffer; };
		inline uint64_t getSize() { return this->size; };

	protected:
		VulkanAllocator* allocator = nullptr;

		VkBuffer buffer;
		VmaAllocation buffer_memory;
		VmaAllocationInfo buffer_memory_info;
		uint64_t size;
	};

	class VulkanVertexBuffer : public VulkanBuffer
	{
	public:
		VulkanVertexBuffer(VulkanAllocator* allocator, VulkanCommandPool* transfer_pool, VkQueue transfer_queue, void* data, uint64_t data_size, VmaMemoryUsage memory_usage, VertexInputDescription& vertex_input_description);

		inline VertexInputDescription& getVertexDescription() { return this->vertex_description; };

	private:
		VertexInputDescription vertex_description;
	};

	class VulkanIndexBuffer : public VulkanBuffer
	{
	public:
		VulkanIndexBuffer(VulkanAllocator* allocator, VulkanCommandPool* transfer_pool, VkQueue transfer_queue, void* data, uint64_t data_size, VmaMemoryUsage memory_usage, uint32_t indices_count, IndexType type);

		inline IndexType getIndicesType() { return this->indices_type; };
		inline uint32_t getIndicesCount() { return this->indices_count; };

	private:
		const IndexType indices_type;
		const uint32_t indices_count;
	};
}