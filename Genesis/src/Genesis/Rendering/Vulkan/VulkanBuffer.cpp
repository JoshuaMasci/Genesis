#include "VulkanBuffer.hpp"

using namespace Genesis;

VulkanBuffer::VulkanBuffer(VulkanAllocator* allocator, uint64_t size_bytes, VkBufferUsageFlags type, VmaMemoryUsage memory_usage)
{
	this->allocator = allocator;

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

VulkanVertexBuffer::VulkanVertexBuffer(VulkanAllocator* allocator, VulkanCommandPool* transfer_pool, VkQueue transfer_queue, void* data, uint64_t data_size, VertexInputDescription& vertex_input_description)
	:VulkanBuffer(allocator, data_size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VMA_MEMORY_USAGE_GPU_ONLY),
	 vertex_description(vertex_input_description)
{
	this->fillBuffer(transfer_pool, transfer_queue, data, data_size);
}

VulkanIndexBuffer::VulkanIndexBuffer(VulkanAllocator* allocator, VulkanCommandPool* transfer_pool, VkQueue transfer_queue, void* data, uint64_t data_size, uint32_t indices_count)
	:VulkanBuffer(allocator, data_size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VMA_MEMORY_USAGE_GPU_ONLY),
	indices_count(indices_count)
{
	this->fillBuffer(transfer_pool, transfer_queue, data, data_size);
}

VulkanUniformBuffer::VulkanUniformBuffer(VkDevice device, VulkanAllocator* allocator, VulkanDescriptorPool* descriptor_pool, VulkanDescriptorSetLayouts* descriptor_layouts, uint64_t size_bytes, uint32_t frames_in_flight)
{
	this->device = device;
	this->allocator = allocator;
	this->descriptor_pool = descriptor_pool;

	VkBufferCreateInfo buffer_info = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
	buffer_info.size = size_bytes;
	buffer_info.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

	VkDescriptorSetLayout layout = descriptor_layouts->getDescriptorSetLayout(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);

	this->buffers.resize(frames_in_flight);
	for (size_t i = 0; i < this->buffers.size(); i++)
	{
		this->allocator->createBuffer(&buffer_info, VMA_MEMORY_USAGE_CPU_TO_GPU, &this->buffers[i].buffer, &this->buffers[i].buffer_memory, &this->buffers[i].buffer_memory_info);

		this->buffers[i].buffer_descriotor_set = this->descriptor_pool->getDescriptorSet(layout);
		VkDescriptorBufferInfo bufferInfo = {};
		bufferInfo.buffer = this->buffers[i].buffer;
		bufferInfo.offset = 0;
		bufferInfo.range = size_bytes;

		VkWriteDescriptorSet descriptorWrite = {};
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.dstSet = this->buffers[i].buffer_descriotor_set;
		descriptorWrite.dstBinding = 0;
		descriptorWrite.dstArrayElement = 0;
		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.pBufferInfo = &bufferInfo;

		vkUpdateDescriptorSets(this->device, 1, &descriptorWrite, 0, nullptr);
	}
}

VulkanUniformBuffer::~VulkanUniformBuffer()
{
	for (size_t i = 0; i < this->buffers.size(); i++)
	{
		this->allocator->destroyBuffer(this->buffers[i].buffer, this->buffers[i].buffer_memory);
		this->descriptor_pool->freeDescriptorSet(this->buffers[i].buffer_descriotor_set);
	}
}

void VulkanUniformBuffer::setData(void* data, uint64_t data_size)
{
	this->data_local = Array<uint8_t>(data_size);
	memcpy(this->data_local.data(), data, data_size);
}

void VulkanUniformBuffer::updateBuffer(uint32_t frame_index)
{
	void* mapped_data;
	this->allocator->mapMemory(this->buffers[frame_index].buffer_memory, &mapped_data);
	memcpy(mapped_data, this->data_local.data(), this->data_local.size());
	this->allocator->unmapMemory(this->buffers[frame_index].buffer_memory);
}
