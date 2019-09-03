#pragma once

#include "Genesis/Core/Types.hpp"
#include "Genesis/Rendering/Vulkan/VulkanDevice.hpp"
#include "Genesis/Rendering/Vulkan/VulkanSwapchain.hpp"
#include "Genesis/Rendering/Vulkan/VulkanAllocator.hpp"
#include "Genesis/Rendering/Vulkan/VulkanSurface.hpp"

namespace Genesis
{
	class VulkanSwapchainFramebuffers
	{
	public:
		VulkanSwapchainFramebuffers(VulkanDevice* device, VulkanSwapchain* swapchain, VulkanSurface* surface, VulkanAllocator* allocator);
		~VulkanSwapchainFramebuffers();

		inline VkFramebuffer getSwapchainFramebuffer(int index) { return this->swapchain_framebuffers[index]; };

		inline VkRenderPass getRenderPass() { return this->render_pass; };

	private:
		VkDevice device;
		VulkanAllocator* allocator;
		VkRenderPass render_pass;

		Array<VkImage> swapchain_images;
		Array<VkImageView> swapchain_imageviews;
		Array<VkFramebuffer> swapchain_framebuffers;

		Array<VkImage> depth_images;
		Array<VkImageView> depth_imageviews;
		Array<VmaAllocation> depth_images_memory;
	};
}