#pragma once

#include "Genesis/Rendering/Vulkan/VulkanInclude.hpp"
#include "Genesis/Rendering/Vulkan/VulkanDevice.hpp"

#include "Genesis/Rendering/RenderingTypes.hpp"
#include "Genesis/Rendering/VertexInputDescription.hpp"

namespace Genesis
{
	class VulkanBuffer
	{
	public:
		VulkanBuffer(VulkanDevice* device, uint64_t size_bytes, VkBufferUsageFlags type, VmaMemoryUsage memory_usage);
		virtual ~VulkanBuffer();

		void fillBuffer(void* data, uint64_t data_size);

		inline VkBuffer get() { return this->buffer; };
		inline uint64_t getSize() { return this->size; };
		inline bool isHostVisable() { return this->host_visable; };

	protected:
		VulkanDevice* device = nullptr;

		VkBuffer buffer;
		VmaAllocation buffer_memory;
		VmaAllocationInfo buffer_memory_info;
		uint64_t size;
		bool host_visable;
	};

	class VulkanVertexBuffer : public VulkanBuffer
	{
	public:
		VulkanVertexBuffer(VulkanDevice* device, uint64_t data_size, VmaMemoryUsage memory_usage, VertexInputDescription& vertex_input_description);

		inline VertexInputDescription& getVertexDescription() { return this->vertex_description; };

	private:
		VertexInputDescription vertex_description;
	};

	class VulkanIndexBuffer : public VulkanBuffer
	{
	public:
		VulkanIndexBuffer(VulkanDevice* device, uint64_t data_size, VmaMemoryUsage memory_usage, IndexType type);

		inline IndexType getIndicesType() { return this->indices_type; };
		inline uint32_t getIndicesCount() { return this->indices_count; };

	private:
		const IndexType indices_type;
		const uint32_t indices_count;
	};


	class VulkanUniformBuffer
	{
	public:
		VulkanUniformBuffer(VulkanDevice* device, uint64_t data_size, VmaMemoryUsage memory_usage, uint32_t frame_count);
		~VulkanUniformBuffer();

	protected:
		List<VulkanBuffer*> buffers;
		uint32_t current_index;
		bool has_changed;
	};
}