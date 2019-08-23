#pragma once

#include "Genesis/Rendering/Vulkan/VulkanInclude.hpp"
#include "Genesis/Rendering/Vulkan/VulkanAllocator.hpp"
#include "Genesis/Rendering/Vulkan/VulkanCommandPool.hpp"
#include "Genesis/Rendering/Vulkan/VulkanDescriptorPool.hpp"
#include "Genesis/Rendering/Vulkan/VulkanDescriptorSetLayouts.hpp"

#include "Genesis/Rendering/VertexInputDescription.hpp"

namespace Genesis
{
	class VulkanBuffer
	{
	public:
		VulkanBuffer(VulkanAllocator* allocator, uint64_t size_bytes, VkBufferUsageFlags type, VmaMemoryUsage memory_usage);
		virtual ~VulkanBuffer();

		void fillBuffer(VulkanCommandPool* transfer_pool, VkQueue transfer_queue, void* data, uint64_t data_size);

		inline VkBuffer get() { return this->buffer; };

	protected:
		VulkanAllocator* allocator = nullptr;

		VkBuffer buffer;
		VmaAllocation buffer_memory;
		VmaAllocationInfo buffer_memory_info;
	};

	class VulkanVertexBuffer : public VulkanBuffer
	{
	public:
		VulkanVertexBuffer(VulkanAllocator* allocator, VulkanCommandPool* transfer_pool, VkQueue transfer_queue, void* data, uint64_t data_size, VertexInputDescription& vertex_input_description);

		inline VertexInputDescription& getVertexDescription() { return this->vertex_description; };

	private:
		VertexInputDescription vertex_description;
	};

	class VulkanIndexBuffer : public VulkanBuffer
	{
	public:
		VulkanIndexBuffer(VulkanAllocator* allocator, VulkanCommandPool* transfer_pool, VkQueue transfer_queue, void* data, uint64_t data_size, uint32_t indices_count);

		inline uint32_t getIndicesCount() { return this->indices_count; };

	private:
		const uint32_t indices_count;
	};


	class VulkanUniformBuffer
	{
	public:
		VulkanUniformBuffer(VkDevice device, VulkanAllocator* allocator, string name, uint64_t size_bytes, uint32_t frames_in_flight);
		~VulkanUniformBuffer();

		void setData(void* data, uint64_t data_size);

		void updateBuffer(uint32_t frame_index);

		inline string getName() { return this->name; };

	private:
		VkDevice device;
		VulkanAllocator* allocator = nullptr;

		//Local copy of data
		Array<uint8_t> data_local;

		string name;
		uint64_t size_bytes;
		struct UniformBuffer
		{
			VkBuffer buffer;
			VmaAllocation buffer_memory;
			VmaAllocationInfo buffer_memory_info;
		};
		Array<UniformBuffer> buffers;
	};
}