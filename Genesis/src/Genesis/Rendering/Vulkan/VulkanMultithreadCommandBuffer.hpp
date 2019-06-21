#pragma once

#include "Genesis/Core/Types.hpp"
#include "Genesis/Rendering/Vulkan/VulkanCommandPool.hpp"
#include "Genesis/Rendering/Vulkan/VulkanInclude.hpp"

namespace Genesis
{
	class VulkanMultithreadCommandBuffer
	{
	public:
		VulkanMultithreadCommandBuffer(VulkanCommandPool* primary_command_pool, Array<VulkanCommandPool*>* secondary_command_pools);
		~VulkanMultithreadCommandBuffer();

		void beginCommandBuffer(VkRenderPassBeginInfo& render_pass_info);
		void endCommandBuffer();
		void submitCommandBuffer(VkQueue queue, Array<VkSemaphore>& wait_semaphores, Array<VkPipelineStageFlags>& wait_states, Array<VkSemaphore>& signal_semaphores, VkFence trigger_fence);

		//Getters
		inline VkCommandBuffer getPrimaryCommandBuffer() { return this->primary_command_buffer; };
		inline VkCommandBuffer getSecondaryCommandBuffer(uint32_t index) { return this->secondary_command_buffers[index]; };

	private:
		VulkanCommandPool* primary_command_pool;
		Array<VulkanCommandPool*>* secondary_command_pools;

		VkCommandBuffer primary_command_buffer;
		Array<VkCommandBuffer> secondary_command_buffers;
	};
}