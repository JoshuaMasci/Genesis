#pragma once

#include "Genesis/Core/Types.hpp"
#include "Genesis/Platform/Window.hpp"
#include "Genesis/Rendering/Vulkan/VulkanInclude.hpp"
#include "Genesis/Rendering/Vulkan/VulkanDevice.hpp"

namespace Genesis
{
	class VulkanSwapchain
	{
	public:
		VulkanSwapchain(VulkanDevice* device, Window* window, VkSurfaceKHR surface);
		~VulkanSwapchain();

		void buildSwapchainFramebuffers(VkRenderPass screen_render_pass);
		void buildSwapchainDepthBuffers(VkFormat depth_format, VmaAllocator allocator);

		inline VkSwapchainKHR getSwapchain() { return this->swapchain; };
		inline VkFormat getSwapchainFormat() { return this->swapchain_image_format; };
		inline VkExtent2D getSwapchainExtent() { return this->swapchain_extent; };

		inline uint32_t getSwapchainImageCount() { return this->swapchain_image_count; };

		inline VkFramebuffer getSwapchainFramebuffer(int index) { return this->swapchain_framebuffers[index]; };

	private:
		VkDevice device;

		VkSwapchainKHR swapchain;
		VkFormat swapchain_image_format;
		VkExtent2D swapchain_extent;

		//Swapchain images
		uint32_t swapchain_image_count;
		vector<VkImage> swapchain_images;
		vector<VkImageView> swapchain_imageviews;
		vector<VkFramebuffer> swapchain_framebuffers;

		vector<VkImage> depth_images;
		vector<VkImageView> depth_imageviews;
		vector<VmaAllocation> depth_images_memory;
	};

	struct VulkanSwapChainSupportDetails
	{
		static VulkanSwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);
		VkSurfaceCapabilitiesKHR capabilities;
		vector<VkSurfaceFormatKHR> formats;
		vector<VkPresentModeKHR> presentModes;
	};
}