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
		VulkanView(VulkanDevice* device, VulkanAllocator* allocator, uint32_t frames_in_flight, VulkanCommandPoolSet* command_pool_set, VkExtent2D size, Array<VkFormat>& color_formats, VkFormat depth_format, VkRenderPass render_pass);
		~VulkanView();

		void startView(uint32_t frame);
		void endView(uint32_t frame);
		void submitView(uint32_t frame, vector<VulkanView*> sub_views, VkFence view_done_fence);

		inline VkExtent2D getViewSize() { return this->size; };
		inline void setClearValues(Array<VkClearValue>& clear) { this->clear_values = clear; };

		inline VkSemaphore getWaitSemaphore(uint32_t i) { return this->view_done_semaphores[i]; };
		inline VulkanMultithreadCommandBuffer* getCommandBuffer(uint32_t i) { return this->command_buffers[i]; };
		inline VulkanFramebuffer* getFramebuffer(uint32_t i) { return this->framebuffers[i]; };

	private:
		VulkanDevice* device = nullptr;
		VkRenderPass render_pass;
		VkExtent2D size;

		Array<VkSemaphore> view_done_semaphores;
		Array<VulkanMultithreadCommandBuffer*> command_buffers;
		Array<VulkanFramebuffer*> framebuffers;

		Array<VkClearValue> clear_values;
	};
}