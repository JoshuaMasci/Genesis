#pragma once

#include "Genesis/Rendering/Vulkan/VulkanDevice.hpp"
#include "Genesis/Core/Types.hpp"

#include <concurrentqueue.h>

namespace Genesis
{
	class VulkanCommandPool
	{
	public:
		VulkanCommandPool(VkDevice device, uint32_t queue_family_index, VkCommandBufferLevel level, VkCommandPoolCreateFlagBits flags);
		~VulkanCommandPool();

		inline VkCommandPool get() { return this->command_pool; };

		VkCommandBuffer getCommandBuffer();
		void releaseCommandBuffer(VkCommandBuffer buffer);

	private:
		VkDevice device;
		VkCommandPool command_pool;
		VkCommandBufferLevel command_buffer_level;


		uint16_t command_buffers_allocated = 0;
		moodycamel::ConcurrentQueue<VkCommandBuffer> command_buffer_queue;
	};
}