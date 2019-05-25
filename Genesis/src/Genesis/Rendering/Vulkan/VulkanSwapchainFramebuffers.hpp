#pragma once

#include "Genesis/Core/Types.hpp"
#include "Genesis/Rendering/Vulkan/VulkanDevice.hpp"
#include "Genesis/Rendering/Vulkan/VulkanSwapchain.hpp"

namespace Genesis
{
	class VulkanSwapchainFramebuffers
	{
	public:
		VulkanSwapchainFramebuffers(VulkanDevice* device, VulkanSwapchain* swapchain, VmaAllocator allocator, VkRenderPass screen_render_pass);
		~VulkanSwapchainFramebuffers();

		inline VkFramebuffer getSwapchainFramebuffer(int index) { return this->swapchain_framebuffers[index]; };

	private:
		VkDevice device;
		VmaAllocator allocator;

		vector<VkImage> swapchain_images;
		vector<VkImageView> swapchain_imageviews;
		vector<VkFramebuffer> swapchain_framebuffers;

		vector<VkImage> depth_images;
		vector<VkImageView> depth_imageviews;
		vector<VmaAllocation> depth_images_memory;
	};
}