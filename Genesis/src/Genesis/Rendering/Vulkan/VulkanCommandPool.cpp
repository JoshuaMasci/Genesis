#include "VulkanCommandPool.hpp"

#include "Genesis/Rendering/Vulkan/VulkanMultithreadCommandBuffer.hpp"

#include <stdexcept>

using namespace Genesis;

VulkanCommandPool::VulkanCommandPool(VkDevice device, uint32_t queue_family_index, VkCommandBufferLevel level, VkCommandPoolCreateFlagBits flags)
{
	this->device = device;

	this->command_buffer_level = level;

	//POOL
	VkCommandPoolCreateInfo pool_info = {};
	pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	pool_info.queueFamilyIndex = queue_family_index;
	pool_info.flags = flags;
	if (vkCreateCommandPool(this->device, &pool_info, nullptr, &this->command_pool) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create graphics command pool!");
	}
}

VulkanCommandPool::~VulkanCommandPool()
{
	vkDestroyCommandPool(this->device, this->command_pool, nullptr);
}

VkCommandBuffer VulkanCommandPool::getCommandBuffer()
{
	VkCommandBuffer buffer;
	bool got_buffer = this->command_buffer_queue.try_dequeue(buffer);
	
	if (!got_buffer)
	{
		VkCommandBufferAllocateInfo alloc_info = {};
		alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		alloc_info.commandPool = this->command_pool;
		alloc_info.level = this->command_buffer_level;
		alloc_info.commandBufferCount = 1;
		if (vkAllocateCommandBuffers(this->device, &alloc_info, &buffer) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to allocate command buffers!");
		}
	}

	return buffer;
}

void VulkanCommandPool::freeCommandBuffer(VkCommandBuffer buffer)
{
	this->command_buffer_queue.enqueue(buffer);
}

VulkanCommandPoolSet::VulkanCommandPoolSet(VulkanDevice* device, uint32_t queue_family_index, uint32_t number_of_threads)
{
	this->primary_command_pool = new VulkanCommandPool(device->get(), queue_family_index, VK_COMMAND_BUFFER_LEVEL_PRIMARY, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
	this->secondary_command_pools = Array<VulkanCommandPool*>(number_of_threads);
	for (int i = 0; i < this->secondary_command_pools.size(); i++)
	{
		this->secondary_command_pools[i] = new VulkanCommandPool(device->get(), queue_family_index, VK_COMMAND_BUFFER_LEVEL_SECONDARY, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
	}
}

VulkanCommandPoolSet::~VulkanCommandPoolSet()
{
	delete this->primary_command_pool;
	for (int i = 0; i < this->secondary_command_pools.size(); i++)
	{
		delete this->secondary_command_pools[i];
	}
}

VulkanMultithreadCommandBuffer* VulkanCommandPoolSet::createCommandBuffer()
{
	return new VulkanMultithreadCommandBuffer(this->primary_command_pool, &this->secondary_command_pools);
}
