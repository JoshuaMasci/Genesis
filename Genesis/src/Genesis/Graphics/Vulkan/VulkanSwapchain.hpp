#pragma once

#include "Genesis/Graphics/Vulkan/VulkanInclude.hpp"
#include "Genesis/Graphics/Vulkan/VulkanDevice.hpp"

namespace Genesis
{
	class VulkanSwapchain
	{
	public:
		VulkanSwapchain(VulkanDevice* device);
		~VulkanSwapchain();
		
	private:
		bool invalid_swapchain = false;

		VkSwapchainKHR swapchain;
		uint32_t swapchain_image_count;
	};	
}