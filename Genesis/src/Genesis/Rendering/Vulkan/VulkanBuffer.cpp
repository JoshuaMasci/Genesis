#include "VulkanBuffer.hpp"

using namespace Genesis;

VulkanBuffer::VulkanBuffer(VulkanDevice* device, uint64_t size_bytes, VkBufferUsageFlags type, VmaMemoryUsage memory_usage)
{
	this->device = device;
	this->size = size_bytes;

	VkBufferCreateInfo buffer_info = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
	buffer_info.size = size_bytes;
	buffer_info.usage = type;

	this->device->createBuffer(&buffer_info, memory_usage, &this->buffer, &this->buffer_memory, &this->buffer_memory_info);
	this->host_visable = this->device->isMemoryHostVisible(this->buffer_memory_info);
}

VulkanBuffer::~VulkanBuffer()
{
	this->device->destroyBuffer(this->buffer, this->buffer_memory);
}

void VulkanBuffer::fillBuffer(void* data, uint64_t data_size)
{
	assert(this->device->isMemoryHostVisible(this->buffer_memory_info));

	void* mapped_data;
	this->device->mapMemory(this->buffer_memory, &mapped_data);
	memcpy_s(mapped_data, this->size, data, data_size);
	this->device->unmapMemory(this->buffer_memory);
}

VulkanVertexBuffer::VulkanVertexBuffer(VulkanDevice* device, uint64_t data_size, VmaMemoryUsage memory_usage, VertexInputDescription& vertex_input_description)
	:VulkanBuffer(device, data_size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, memory_usage)
{
	this->vertex_description = vertex_input_description;
}

VulkanIndexBuffer::VulkanIndexBuffer(VulkanDevice* device, uint64_t data_size, VmaMemoryUsage memory_usage, IndexType type)
	:VulkanBuffer(device, data_size, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, memory_usage),
	indices_type(type),
	indices_count((uint32_t)(data_size / ((type == IndexType::uint32) ? 4 : 2)))
{

}

VulkanUniformBuffer::VulkanUniformBuffer(VulkanDevice* device, uint64_t data_size, VmaMemoryUsage memory_usage, uint32_t frame_count)
{
	this->buffers.resize(frame_count);
	for (size_t i = 0; i < this->buffers.size(); i++)
	{
		this->buffers[i] = new VulkanBuffer(device, data_size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, memory_usage);
	}
}

VulkanUniformBuffer::~VulkanUniformBuffer()
{
	for (size_t i = 0; i < this->buffers.size(); i++)
	{
		delete this->buffers[i];
	}
}
