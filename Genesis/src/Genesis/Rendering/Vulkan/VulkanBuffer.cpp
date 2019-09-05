#include "VulkanBuffer.hpp"

using namespace Genesis;

VulkanBuffer::VulkanBuffer(VulkanAllocator* allocator, uint64_t size_bytes, VkBufferUsageFlags type, VmaMemoryUsage memory_usage)
{
	this->allocator = allocator;
	this->size = size_bytes;

	VkBufferCreateInfo buffer_info = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
	buffer_info.size = size_bytes;
	buffer_info.usage = type;

	this->allocator->createBuffer(&buffer_info, memory_usage, &this->buffer, &this->buffer_memory, &this->buffer_memory_info);
}

VulkanBuffer::~VulkanBuffer()
{
	this->allocator->destroyBuffer(this->buffer, this->buffer_memory);
}

void VulkanBuffer::fillBuffer(VulkanCommandPool* transfer_pool, VkQueue transfer_queue, void* data, uint64_t data_size)
{
	VkDeviceSize buffer_size = this->buffer_memory_info.size;

	if (this->allocator->isMemoryHostVisible(this->buffer_memory_info))
	{
		void* mapped_data;
		this->allocator->mapMemory(this->buffer_memory, &mapped_data);
		memcpy(mapped_data, data, data_size);
		this->allocator->unmapMemory(this->buffer_memory);
	}
	else
	{
		//Staging Buffer
		VkBuffer staging_buffer;
		VmaAllocation staging_buffer_memory;
		VkBufferCreateInfo bufferInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
		bufferInfo.size = data_size;
		bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		this->allocator->createBuffer(&bufferInfo, VMA_MEMORY_USAGE_CPU_ONLY, &staging_buffer, &staging_buffer_memory, nullptr);

		void* mapped_data;
		this->allocator->mapMemory(staging_buffer_memory, &mapped_data);
		memcpy(mapped_data, data, data_size);
		this->allocator->unmapMemory(staging_buffer_memory);

		VkCommandBuffer command_buffer = transfer_pool->getCommandBuffer();
		VkCommandBufferBeginInfo begin_info = {};
		begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		vkBeginCommandBuffer(command_buffer, &begin_info);

		VkBufferCopy copyRegion = {};
		copyRegion.srcOffset = 0; // Optional
		copyRegion.dstOffset = 0; // Optional
		copyRegion.size = data_size;
		vkCmdCopyBuffer(command_buffer, staging_buffer, this->buffer, 1, &copyRegion);

		vkEndCommandBuffer(command_buffer);
		VkSubmitInfo submit_info = {};
		submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submit_info.commandBufferCount = 1;
		submit_info.pCommandBuffers = &command_buffer;

		//TODO better waiting
		vkQueueSubmit(transfer_queue, 1, &submit_info, VK_NULL_HANDLE);
		vkQueueWaitIdle(transfer_queue);
		transfer_pool->freeCommandBuffer(command_buffer);

		this->allocator->destroyBuffer(staging_buffer, staging_buffer_memory);
	}
}

VulkanVertexBuffer::VulkanVertexBuffer(VulkanAllocator* allocator, VulkanCommandPool* transfer_pool, VkQueue transfer_queue, void* data, uint64_t data_size, VmaMemoryUsage memory_usage, VertexInputDescription& vertex_input_description)
	:VulkanBuffer(allocator, data_size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, memory_usage),
	 vertex_description(vertex_input_description)
{
	this->fillBuffer(transfer_pool, transfer_queue, data, data_size);
}

VulkanIndexBuffer::VulkanIndexBuffer(VulkanAllocator* allocator, VulkanCommandPool* transfer_pool, VkQueue transfer_queue, void* data, uint64_t data_size, VmaMemoryUsage memory_usage, uint32_t indices_count, IndexType type)
	:VulkanBuffer(allocator, data_size, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, memory_usage),
	indices_type(type),
	indices_count(indices_count)
{
	this->fillBuffer(transfer_pool, transfer_queue, data, data_size);
}
