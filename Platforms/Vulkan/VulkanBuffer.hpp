#pragma once

#include "VulkanInclude.hpp"
#include "VulkanDevice.hpp"

#include "Genesis/Rendering/RenderingTypes.hpp"
#include "Genesis/Rendering/VertexInputDescription.hpp"

namespace Genesis
{
	class VulkanBuffer
	{
	public:
		VulkanBuffer() {};
		VulkanBuffer(VulkanDevice* device, uint64_t size_bytes, VkBufferUsageFlags type, VmaMemoryUsage memory_usage);
		virtual ~VulkanBuffer();

		void fillBuffer(void* data, uint64_t data_size);

		inline VkBuffer get() { return this->buffer; };
		inline uint64_t getSize() { return this->size; };
		inline bool isHostVisable() { return this->host_visable; };
		inline bool inTransfer() { return this->in_transfer; };

	protected:
		VulkanDevice* device = nullptr;

		VkBuffer buffer = VK_NULL_HANDLE;
		VmaAllocation buffer_memory = VK_NULL_HANDLE;
		VmaAllocationInfo buffer_memory_info = {};
		uint64_t size = 0;
		bool host_visable = false;
		bool in_transfer = false;

		friend class VulkanTransferBuffer;
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