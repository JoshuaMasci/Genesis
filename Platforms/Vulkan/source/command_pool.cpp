#include "command_pool.hpp"

namespace genesis
{
	CommandPool::CommandPool(VkDevice device, uint32_t queue_family_index, VkCommandBufferLevel level, VkCommandPoolCreateFlagBits flags)
	{
		this->device = device;

		this->command_buffer_level = level;

		//POOL
		VkCommandPoolCreateInfo pool_info = {};
		pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		pool_info.queueFamilyIndex = queue_family_index;
		pool_info.flags = flags;
		VK_ASSERT(vkCreateCommandPool(this->device, &pool_info, nullptr, &this->command_pool));
	}

	CommandPool::~CommandPool()
	{
		vkDestroyCommandPool(this->device, this->command_pool, nullptr);
	}
	
	VkCommandBuffer CommandPool::get_command_buffer()
	{
		VkCommandBuffer command_buffer;
		if (!this->freed_command_buffers.empty())
		{
			command_buffer = this->freed_command_buffers.back();
			vkResetCommandBuffer(command_buffer, 0);
			this->freed_command_buffers.pop_back();
		}
		else
		{
			VkCommandBufferAllocateInfo alloc_info = {};
			alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			alloc_info.commandPool = this->command_pool;
			alloc_info.level = this->command_buffer_level;
			alloc_info.commandBufferCount = 1;
			VK_ASSERT(vkAllocateCommandBuffers(this->device, &alloc_info, &command_buffer));
		}

		return command_buffer;
	}
	
	void CommandPool::free_command_buffer(VkCommandBuffer command_buffer)
	{
		this->freed_command_buffers.push_back(command_buffer);
	}
}