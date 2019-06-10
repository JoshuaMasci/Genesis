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

		//For memory operations
		VkCommandPool transfer_command_pool;

		VkCommandBuffer startTransferCommandBuffer();
		void endTransferCommandBuffer(VkCommandBuffer command_buffer);

	private:
		VulkanDevice* device;
	};
}