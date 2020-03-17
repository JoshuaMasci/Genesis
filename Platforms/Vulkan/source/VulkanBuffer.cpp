#include "VulkanBuffer.hpp"

using namespace Genesis;

VulkanBuffer::VulkanBuffer(VulkanDevice* device, uint64_t size_bytes, VkBufferUsageFlags buffer_usage, VmaMemoryUsage memory_usage)
{
	this->device = device;
	this->size = size_bytes;

	VkBufferCreateInfo buffer_info = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
	buffer_info.size = size_bytes;
	buffer_info.usage = buffer_usage;

	VmaAllocationInfo buffer_memory_info = {};
	this->device->createBuffer(&buffer_info, memory_usage, &this->buffer, &this->buffer_memory, &buffer_memory_info);
	this->host_visable = this->device->isMemoryHostVisible(buffer_memory_info);
}

VulkanBuffer::~VulkanBuffer()
{
	if (this->buffer != VK_NULL_HANDLE)
	{
		this->device->destroyBuffer(this->buffer, this->buffer_memory);
	}
}

void VulkanBuffer::fillBuffer(void* data, uint64_t data_size)
{
	assert(this->host_visable);

	void* mapped_data;
	this->device->mapMemory(this->buffer_memory, &mapped_data);
	memcpy_s(mapped_data, this->size, data, data_size);
	this->device->unmapMemory(this->buffer_memory);
}

Genesis::VulkanDynamicBuffer::VulkanDynamicBuffer(VulkanDevice * device, uint64_t data_size, VkBufferUsageFlags buffer_usage, VmaMemoryUsage memory_usage, uint32_t frame_count)
{
	this->buffers.resize(frame_count);
	for (size_t i = 0; i < this->buffers.size(); i++)
	{
		this->buffers[i] = new VulkanBuffer(device, data_size, buffer_usage, memory_usage);
	}

	this->size = data_size;
}

VulkanDynamicBuffer::~VulkanDynamicBuffer()
{
	for (size_t i = 0; i < this->buffers.size(); i++)
	{
		delete this->buffers[i];
	}
}

void VulkanDynamicBuffer::incrementIndex()
{
	this->current_index = ((this->current_index + 1) % this->buffers.size());
}
