#pragma once

#include "vulkan_include.hpp"
#include "device.hpp"

namespace genesis
{
	class CommandPool
	{
	protected:
		VkDevice device;
		VkCommandPool command_pool;
		VkCommandBufferLevel command_buffer_level;

		vector<VkCommandBuffer> freed_command_buffers;

	public:
		CommandPool(VkDevice device, uint32_t queue_family_index, VkCommandBufferLevel level, VkCommandPoolCreateFlagBits flags);
		~CommandPool();

		inline VkCommandPool get() { return this->command_pool; };

		VkCommandBuffer get_command_buffer();
		void free_command_buffer(VkCommandBuffer command_buffer);
	};
}