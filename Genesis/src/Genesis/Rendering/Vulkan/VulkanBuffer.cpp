#include "VulkanBuffer.hpp"

#include <stdio.h>

using namespace Genesis;

VulkanBuffer::VulkanBuffer(VulkanInstance* instance, uint64_t size_bytes, VkBufferUsageFlags usage, VmaMemoryUsage memory_usage)
{
	this->instance = instance;
	this->size = size_bytes;

	VkBufferCreateInfo buffer_info = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
	buffer_info.size = size_bytes;
	buffer_info.usage = usage;

	VmaAllocationCreateInfo alloc_info = {};
	alloc_info.usage = memory_usage;

	vmaCreateBuffer(this->instance->allocator, &buffer_info, &alloc_info, &this->buffer, &this->buffer_memory, &this->buffer_memory_info);
}

VulkanBuffer::~VulkanBuffer()
{
	vmaDestroyBuffer(this->instance->allocator, this->buffer, this->buffer_memory);
}

void VulkanBuffer::fill(void* data, uint64_t data_size)
{
	VkDeviceSize buffer_size = this->buffer_memory_info.size;

	VkMemoryPropertyFlags memFlags;
	vmaGetMemoryTypeProperties(this->instance->allocator, this->buffer_memory_info.memoryType, &memFlags);
	if ((memFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) != 0)
	{
		void* mappedData;
		vmaMapMemory(this->instance->allocator, this->buffer_memory, &mappedData);
		memcpy(mappedData, data, data_size);
		vmaUnmapMemory(this->instance->allocator, this->buffer_memory);
	}
	else
	{
		//Staging Buffer
		VkBuffer staging_buffer;
		VmaAllocation staging_buffer_memory;
		VkBufferCreateInfo bufferInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
		bufferInfo.size = data_size;
		bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		VmaAllocationCreateInfo allocInfo = {};
		allocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;
		vmaCreateBuffer(this->instance->allocator, &bufferInfo, &allocInfo, &staging_buffer, &staging_buffer_memory, nullptr);

		void* mappedData;
		vmaMapMemory(this->instance->allocator, staging_buffer_memory, &mappedData);
		memcpy(mappedData, data, data_size);
		vmaUnmapMemory(this->instance->allocator, staging_buffer_memory);

		//TODO use reuseable command buffer
	
		VkCommandBuffer command_buffer = this->instance->command_pool->startTransferCommandBuffer();

		VkBufferCopy copyRegion = {};
		copyRegion.srcOffset = 0; // Optional
		copyRegion.dstOffset = 0; // Optional
		copyRegion.size = data_size;
		vkCmdCopyBuffer(command_buffer, staging_buffer, this->buffer, 1, &copyRegion);
	
		this->instance->command_pool->endTransferCommandBuffer(command_buffer);

		vmaDestroyBuffer(this->instance->allocator, staging_buffer, staging_buffer_memory);
	}
}
