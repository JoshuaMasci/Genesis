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

		inline VkSwapchainKHR get() { return this->swapchain; };
		inline VkFormat getSwapchainFormat() { return this->swapchain_image_format; };
		inline VkFormat getSwapchainDepthFormat() { return this->swapchain_depth_format; };
		inline VkExtent2D getSwapchainExtent() { return this->swapchain_extent; };
		inline uint32_t getSwapchainImageCount() { return this->swapchain_image_count; };

	private:
		VkDevice device;

		VkSwapchainKHR swapchain;
		VkFormat swapchain_image_format;
		VkFormat swapchain_depth_format;
		VkExtent2D swapchain_extent;
		uint32_t swapchain_image_count;
	};

	struct VulkanSwapChainSupportDetails
	{
		static VulkanSwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);
		VkSurfaceCapabilitiesKHR capabilities;
		vector<VkSurfaceFormatKHR> formats;
		vector<VkPresentModeKHR> presentModes;
	};
}