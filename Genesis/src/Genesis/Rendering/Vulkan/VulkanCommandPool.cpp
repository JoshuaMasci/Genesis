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
