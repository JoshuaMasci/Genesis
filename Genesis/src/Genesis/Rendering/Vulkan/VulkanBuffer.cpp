#include "VulkanBuffer.hpp"

#include <stdio.h>

using namespace Genesis;

VulkanBuffer::VulkanBuffer(VmaAllocator allocator, uint32_t size_bytes, VkBufferUsageFlags usage, VmaMemoryUsage memory_usage)
{
	this->allocator = allocator;
	this->size = size_bytes;

	VkBufferCreateInfo bufferInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
	bufferInfo.size = size_bytes;
	bufferInfo.usage = usage;

	VmaAllocationCreateInfo allocInfo = {};
	allocInfo.usage = memory_usage;

	vmaCreateBuffer(allocator, &bufferInfo, &allocInfo, &this->buffer, &this->buffer_memory, &this->buffer_memory_info);
}

VulkanBuffer::~VulkanBuffer()
{
	vmaDestroyBuffer(this->allocator, this->buffer, this->buffer_memory);
}

void VulkanBuffer::fillBuffer(void* data, uint32_t data_size)
{
	VkMemoryPropertyFlags memFlags;
	vmaGetMemoryTypeProperties(this->allocator, this->buffer_memory_info.memoryType, &memFlags);
	if ((memFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) != 0)
	{
		void* mappedData;
		vmaMapMemory(allocator, this->buffer_memory, &mappedData);
		memcpy(mappedData, data, data_size);
		vmaUnmapMemory(allocator, this->buffer_memory);
	}
	else
	{
		//TODO: Copy memory
		printf("Error: can't copy memory");
	}
}
