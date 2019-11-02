#pragma once

#include "Genesis/Rendering/FramebufferLayout.hpp"

#include "Genesis/Rendering/Vulkan/VulkanInclude.hpp"
#include "Genesis/Rendering/Vulkan/VulkanDevice.hpp"
#include "Genesis/Rendering/Vulkan/VulkanAllocator.hpp"
#include "Genesis/Rendering/Vulkan/VulkanMultithreadCommandBuffer.hpp"
#include "Genesis/Rendering/Vulkan/VulkanFramebuffer.hpp"
#include "Genesis/Rendering/Vulkan/VulkanDescriptorPool.hpp"

namespace Genesis
{
	class VulkanView
	{
	public:
		VulkanView(VulkanDevice* device, VulkanAllocator* allocator, uint32_t frames_in_flight, VkExtent2D size, Array<VkFormat>& color_formats, VkFormat depth_format, VkRenderPass render_pass, Array<VulkanCommandBuffer*> command_buffers, VkSampler sampler);
		~VulkanView();

		void startView(uint32_t frame);
		void endView(uint32_t frame);
		void submitView(uint32_t frame, vector<VulkanView*> sub_views, VkFence view_done_fence);

		void setViewSize(VkExtent2D size);
		inline VkExtent2D getViewSize() { return this->size; };
		
		inline void setClearValues(Array<VkClearValue>& clear) { this->clear_values = clear; };

		inline VkSemaphore getWaitSemaphore(uint32_t frame) { return this->view_done_semaphores[frame]; };
		inline VulkanCommandBuffer* getCommandBuffer(uint32_t frame) { return this->command_buffers[frame]; };
		inline VulkanFramebuffer* getFramebuffer(uint32_t frame) { return this->framebuffers[frame]; };

		VkSampler getSampler() { return this->sampler; };

	private:
		VulkanDevice* device = nullptr;
		VulkanAllocator* allocator = nullptr;
		VkSampler sampler;
		VkRenderPass render_pass;
		VkExtent2D size;

		Array<VkFormat> color_formats;
		VkFormat depth_format;

		Array<VkSemaphore> view_done_semaphores;
		Array<VulkanCommandBuffer*> command_buffers;
		Array<VulkanFramebuffer*> framebuffers;

		Array<VkClearValue> clear_values;
	};
}