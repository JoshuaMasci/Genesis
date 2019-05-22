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

		inline VkCommandPool getGraphicsCommandPool() { return this->graphics_command_pool; };
		inline VkCommandPool getTransferCommandPool() { return this->transfer_command_pool; };

	private:
		VulkanDevice* device;

		//For graphics and present commands
		VkCommandPool graphics_command_pool;

		//Per thread command buffers
		vector<VkCommandBuffer> graphics_command_buffers;

		//For memory operations
		VkCommandPool transfer_command_pool;
	};
}