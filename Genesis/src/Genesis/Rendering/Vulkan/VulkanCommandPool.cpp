#include "VulkanCommandPool.hpp"

#include <stdexcept>

using namespace Genesis;

VulkanCommandPool::VulkanCommandPool(VulkanDevice* device, uint32_t number_of_threads)
{
	this->device = device;

	//POOL
	VkCommandPoolCreateInfo graphics_pool_info = {};
	graphics_pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	graphics_pool_info.queueFamilyIndex = this->device->getGraphicsFamilyIndex();
	graphics_pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	if (vkCreateCommandPool(this->device->getDevice(), &graphics_pool_info, nullptr, &this->graphics_command_pool) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create graphics command pool!");
	}

	//Transfer
	VkCommandPoolCreateInfo transfer_pool_info = {};
	transfer_pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	transfer_pool_info.queueFamilyIndex = this->device->getTransferFamilyIndex();
	transfer_pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	if (vkCreateCommandPool(this->device->getDevice(), &transfer_pool_info, nullptr, &this->transfer_command_pool) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create transfer command pool!");
	}
}

VulkanCommandPool::~VulkanCommandPool()
{
	vkDestroyCommandPool(this->device->getDevice(), this->graphics_command_pool, nullptr);
	vkDestroyCommandPool(this->device->getDevice(), this->transfer_command_pool, nullptr);
}

VkCommandBuffer VulkanCommandPool::startTransferCommandBuffer()
{
	VkCommandBufferAllocateInfo command_alloc_info = {};
	command_alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	command_alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	command_alloc_info.commandPool = this->transfer_command_pool;
	command_alloc_info.commandBufferCount = 1;

	VkCommandBuffer command_buffer;
	vkAllocateCommandBuffers(this->device->getDevice(), &command_alloc_info, &command_buffer);

	VkCommandBufferBeginInfo begin_info = {};
	begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(command_buffer, &begin_info);

	return command_buffer;
}

void VulkanCommandPool::endTransferCommandBuffer(VkCommandBuffer command_buffer)
{
	vkEndCommandBuffer(command_buffer);

	VkSubmitInfo submit_info = {};
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit_info.commandBufferCount = 1;
	submit_info.pCommandBuffers = &command_buffer;

	VkQueue queue = this->device->getTransferQueue();
	vkQueueSubmit(queue, 1, &submit_info, VK_NULL_HANDLE);
	vkQueueWaitIdle(queue);

	vkFreeCommandBuffers(this->device->getDevice(), this->transfer_command_pool, 1, &command_buffer);
}
