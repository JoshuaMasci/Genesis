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
		void freeCommandBuffer(VkCommandBuffer buffer);

	private:
		VkDevice device;
		VkCommandPool command_pool;
		VkCommandBufferLevel command_buffer_level;

		moodycamel::ConcurrentQueue<VkCommandBuffer> command_buffer_queue;
	};

	//Prototype
	class VulkanMultithreadCommandBuffer;

	class VulkanCommandPoolSet
	{
	public:
		VulkanCommandPoolSet(VulkanDevice* device, uint32_t queue_family_index, uint32_t number_of_threads);
		~VulkanCommandPoolSet();

		inline VulkanCommandPool* getPrimaryCommandPool() { return this->primary_command_pool; };
		inline VulkanCommandPool* getSecondaryCommandPool(uint32_t thread) { return this->secondary_command_pools[thread]; };

		VulkanMultithreadCommandBuffer* createCommandBuffer();

	private:
		VulkanCommandPool* primary_command_pool = nullptr;
		Array<VulkanCommandPool*> secondary_command_pools;
	};
}