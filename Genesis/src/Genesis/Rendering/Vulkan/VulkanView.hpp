#pragma once

#include "Genesis/Rendering/Vulkan/VulkanInclude.hpp"
#include "Genesis/Rendering/Vulkan/VulkanDevice.hpp"
#include "Genesis/Rendering/Vulkan/VulkanAllocator.hpp"
#include "Genesis/Rendering/Vulkan/VulkanMultithreadCommandBuffer.hpp"
#include "Genesis/Rendering/Vulkan/VulkanFramebuffer.hpp"

namespace Genesis
{
	class VulkanView
	{
	public:
		VulkanView(VulkanDevice* device, VulkanAllocator* allocator, VulkanMultithreadCommandBuffer* command_buffer, VkExtent2D size, VulkanFramebufferLayout* layout);
		~VulkanView();

		void startView();
		void endView();
		void submitView(vector<VulkanView*> sub_views, VkFence view_done_fence);

		inline VkSemaphore getWaitSemaphore() { return this->view_done_semaphore; };
		inline VkExtent2D getViewSize() { return this->framebuffer->getSize(); };

	private:
		VulkanDevice* device = nullptr;
		VkSemaphore view_done_semaphore = VK_NULL_HANDLE;

		VulkanMultithreadCommandBuffer* command_buffer = nullptr;

		VkRenderPass render_pass;
		VulkanFramebuffer* framebuffer = nullptr;
	};
}