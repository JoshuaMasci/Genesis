#pragma once

#include "Genesis/Core/Types.hpp"
#include "Genesis/Rendering/Vulkan/VulkanCommandPool.hpp"
#include "Genesis/Rendering/Vulkan/VulkanCommandBuffer.hpp"
#include "Genesis/Rendering/Vulkan/VulkanThread.hpp"
#include "Genesis/Rendering/Vulkan/VulkanInclude.hpp"

namespace Genesis
{
	class VulkanMultithreadCommandBuffer
	{
	public:
		VulkanMultithreadCommandBuffer(uint32_t frame_index, VkDevice device, VulkanCommandPool* primary_command_pool, VulkanPipelinePool* pipeline_pool, Array<VulkanThread>& threads, VulkanUniformPool* uniform_pool, VkSampler temp_sampler);
		~VulkanMultithreadCommandBuffer();

		void beginCommandBufferPrimary(VkRenderPassBeginInfo& render_pass_info);
		void endCommandBuffer();
		void submitCommandBuffer(VkQueue queue, Array<VkSemaphore>& wait_semaphores, Array<VkPipelineStageFlags>& wait_states, Array<VkSemaphore>& signal_semaphores, VkFence trigger_fence);

	private:
		const uint32_t frame_index;
		VkCommandBuffer primary_command_buffer;
		VulkanCommandPool* primary_command_pool;

		Array<VulkanCommandBuffer*> secondary_command_buffers;
	};
}