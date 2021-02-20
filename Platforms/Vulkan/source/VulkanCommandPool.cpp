#include "VulkanCommandPool.hpp"

#include "Genesis/Debug/Assert.hpp"

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
	GENESIS_ENGINE_ASSERT_ERROR((vkCreateCommandPool(this->device, &pool_info, nullptr, &this->command_pool) == VK_SUCCESS), "failed to create graphics command pool");
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
		GENESIS_ENGINE_ASSERT_ERROR((vkAllocateCommandBuffers(this->device, &alloc_info, &buffer) == VK_SUCCESS), "failed to allocate command buffers")
	}

	return buffer;
}

void VulkanCommandPool::freeCommandBuffer(VkCommandBuffer buffer)
{
	this->command_buffer_queue.enqueue(buffer);
}