#pragma once

#include "Genesis/Core/Types.hpp"
#include "Genesis/Platform/Window.hpp"
#include "Genesis/Rendering/Vulkan/VulkanInclude.hpp"
#include "Genesis/Rendering/Vulkan/VulkanDevice.hpp"
#include "Genesis/Rendering/Vulkan/VulkanSurface.hpp"

namespace Genesis
{
	class VulkanSwapchain
	{
	public:
		VulkanSwapchain(VulkanDevice* device, VkExtent2D surface_size, VulkanSurface* surface);
		~VulkanSwapchain();

		inline VkSwapchainKHR get() { return this->swapchain; };
		inline VkExtent2D getSwapchainExtent() { return this->size; };
		inline uint32_t getImageCount() { return this->image_count; };
	private:
		VkDevice device;

		VkSwapchainKHR swapchain;
		uint32_t image_count;
		VkExtent2D size;
	};
}