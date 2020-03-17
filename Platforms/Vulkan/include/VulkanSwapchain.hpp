#pragma once

#include "Genesis/Core/Types.hpp"
#include "Genesis/Platform/Window.hpp"
#include "VulkanInclude.hpp"
#include "VulkanDevice.hpp"

namespace Genesis
{
	class VulkanSwapchain
	{
	public:
		VulkanSwapchain(VulkanDevice* device, VkExtent2D surface_size, VkSurfaceKHR surface);
		~VulkanSwapchain();

		void invalidateSwapchain();
		void recreateSwapchain(VkExtent2D surface_size);

		uint32_t getNextImage(VkSemaphore image_ready_semaphore);

		inline bool invalid() { return this->swapchain_invalid; };

		inline VkSwapchainKHR get() { return this->current.swapchain; };
		inline VkExtent2D getSwapchainExtent() { return this->current.size; };
		inline uint32_t getImageCount() { return this->current.image_count; };
		inline VkRenderPass getRenderPass() { return this->current.render_pass; };

		inline VkFramebuffer getFramebuffer(uint32_t image_index) { return this->current.framebuffers[image_index]; };

	private:
		VulkanDevice* device;
		VkSurfaceKHR surface;

		bool swapchain_invalid = false;

		struct Swapchain
		{
			VkSwapchainKHR swapchain;
			uint32_t image_count;
			VkExtent2D size;
			VkRenderPass render_pass;
			List<VkImage> images;
			List<VkImageView> image_views;
			List<VkFramebuffer> framebuffers;
		};

		Swapchain createSwapchain(VkPhysicalDevice physical_device, VkExtent2D surface_size, VkSurfaceKHR surface, VkSwapchainKHR old_swapchain);
		void destroySwapchain(Swapchain& swapchain);

		Swapchain current;
		Swapchain old;
	};
}