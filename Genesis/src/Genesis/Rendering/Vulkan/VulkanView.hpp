#pragma once

#include "Genesis/Rendering/CommandBuffer.hpp"
#include "Genesis/Rendering/Vulkan/VulkanFramebuffer.hpp"
#include "Genesis/Rendering/Vulkan/VulkanCommanBuffer.hpp"

namespace Genesis
{
	class VulkanViewSingleThread
	{
	public:
		VulkanViewSingleThread(VulkanDevice* device, uint32_t frame_count, VulkanCommandPool* command_pool, VulkanThreadPipelinePool* pipeline_pool, VulkanDescriptorPool* descriptor_pool, VulkanSamplerPool* sampler_pool, List<VulkanTransferBuffer*> transfer_buffers, VkExtent2D size, List<VkFormat>& color_formats, VkFormat depth_format, VkRenderPass render_pass);
		~VulkanViewSingleThread();

		void start(uint32_t frame_index);
		void end();

		inline VulkanFramebuffer* getFramebuffer(uint32_t frame_index) { return this->frames[frame_index].frameBuffer; };
		inline VulkanCommandBuffer* getCommandBuffer(uint32_t frame_index) { return this->frames[frame_index].command_buffer; };

		inline void setSize(VkExtent2D new_size) { this->size = new_size; };

	private:
		VulkanDevice* device = nullptr;

		uint32_t frame_index;

		struct ViewFrame
		{
			VulkanFramebuffer* frameBuffer = nullptr;
			VulkanCommandBuffer* command_buffer = nullptr;
		};

		List<ViewFrame> frames;

		VkExtent2D size;
		List<VkFormat> color_formats;
		VkFormat depth_format;
		List<VkClearValue> clear_values;
		VkRenderPass render_pass = VK_NULL_HANDLE;
	};

	class VulkanViewMultithread
	{
	private:
		VulkanDevice* device = nullptr;

		uint32_t frame_index;

		struct ViewFrame
		{
			VulkanFramebuffer* frameBuffer = nullptr;
			VulkanCommandBuffer* command_buffer;
		};
	};
}