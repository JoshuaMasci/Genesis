#include "VulkanCommandPool.hpp"

#include <stdexcept>

Genesis::VulkanCommandPool::VulkanCommandPool(VulkanDevice* device, uint32_t number_of_threads)
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

	//PRIMARY
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = this->graphics_command_pool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = 1;
	if (vkAllocateCommandBuffers(this->device->getDevice(), &allocInfo, &this->graphics_command_buffer) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate command buffers!");
	}

	//PRIMARY SEMAPHORE
	VkSemaphoreCreateInfo semaphoreInfo = {};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	if (vkCreateSemaphore(this->device->getDevice(), &semaphoreInfo, nullptr, &this->graphics_command_buffer_finished) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create semaphores!");
	}

	//SECONDARY
	this->graphics_secondary_command_buffers.resize(number_of_threads);
	VkCommandBufferAllocateInfo secondary_buffers_info = {};
	secondary_buffers_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	secondary_buffers_info.commandPool = this->graphics_command_pool;
	secondary_buffers_info.level = VK_COMMAND_BUFFER_LEVEL_SECONDARY;
	secondary_buffers_info.commandBufferCount = (uint32_t)this->graphics_secondary_command_buffers.size();
	if (vkAllocateCommandBuffers(this->device->getDevice(), &secondary_buffers_info, this->graphics_secondary_command_buffers.data()) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate command buffers!");
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

Genesis::VulkanCommandPool::~VulkanCommandPool()
{
	vkDestroyCommandPool(this->device->getDevice(), this->graphics_command_pool, nullptr);
	vkDestroySemaphore(this->device->getDevice(), this->graphics_command_buffer_finished, nullptr);

	vkDestroyCommandPool(this->device->getDevice(), this->transfer_command_pool, nullptr);
}
