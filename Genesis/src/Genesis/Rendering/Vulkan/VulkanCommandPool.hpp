#pragma once

#include "Genesis/Rendering/Vulkan/VulkanDevice.hpp"
#include "Genesis/Core/Types.hpp"

namespace Genesis
{
	class VulkanCommandPool
	{
	public:
		VulkanCommandPool(VulkanDevice* device, uint32_t number_of_threads);
		~VulkanCommandPool();

		//For graphics and present commands
		VkCommandPool graphics_command_pool;

		VkCommandBuffer graphics_command_buffer;
		VkSemaphore graphics_command_buffer_finished;

		//Per thread command buffers
		vector<VkCommandBuffer> graphics_secondary_command_buffers;

		//For memory operations
		VkCommandPool transfer_command_pool;

	private:
		VulkanDevice* device;
	};
}