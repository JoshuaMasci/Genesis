#include "VulkanBuffer.hpp"

#include <stdio.h>

using namespace Genesis;

VulkanBuffer::VulkanBuffer(VulkanInstance* instance, uint64_t size_bytes, VkBufferUsageFlags usage, VmaMemoryUsage memory_usage)
{
	this->instance = instance;
	this->size = size_bytes;

	VkBufferCreateInfo bufferInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
	bufferInfo.size = size_bytes;
	bufferInfo.usage = usage;

	VmaAllocationCreateInfo allocInfo = {};
	allocInfo.usage = memory_usage;

	vmaCreateBuffer(this->instance->allocator, &bufferInfo, &allocInfo, &this->buffer, &this->buffer_memory, &this->buffer_memory_info);
}

VulkanBuffer::~VulkanBuffer()
{
	vmaDestroyBuffer(this->instance->allocator, this->buffer, this->buffer_memory);
}

void VulkanBuffer::fillBuffer(void* data, uint64_t data_size)
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
		VkCommandBufferAllocateInfo commandAllocInfo = {};
		commandAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		commandAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		commandAllocInfo.commandPool = this->instance->command_pool->transfer_command_pool;
		commandAllocInfo.commandBufferCount = 1;
		VkCommandBuffer commandBuffer;
		vkAllocateCommandBuffers(this->instance->device->getDevice(), &commandAllocInfo, &commandBuffer);

		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		vkBeginCommandBuffer(commandBuffer, &beginInfo);
		VkBufferCopy copyRegion = {};
		copyRegion.srcOffset = 0; // Optional
		copyRegion.dstOffset = 0; // Optional
		copyRegion.size = data_size;
		vkCmdCopyBuffer(commandBuffer, staging_buffer, this->buffer, 1, &copyRegion);
		vkEndCommandBuffer(commandBuffer);

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		VkQueue queue = this->instance->device->getTransferQueue();
		vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(queue);

		vmaDestroyBuffer(this->instance->allocator, staging_buffer, staging_buffer_memory);
		vkFreeCommandBuffers(this->instance->device->getDevice(),this->instance->command_pool->transfer_command_pool, 1, &commandBuffer);
	}
}
