#pragma once

#include "vulkan_include.hpp"
#include "device.hpp"

#include <vector>

namespace genesis
{
	class Swapchain
	{
	protected:
		Device* device = nullptr;
		VkSurfaceKHR surface = VK_NULL_HANDLE;

		VkSwapchainKHR swapchain = VK_NULL_HANDLE;
		std::vector<VkImage> images;
		VkExtent2D image_extent;

		VkSwapchainKHR create_swapchain(VkExtent2D surface_size);

	public:
		Swapchain(Device* device, VkSurfaceKHR surface, VkExtent2D surface_size);
		~Swapchain();

		uint32_t get_next_image(VkSemaphore image_ready_semaphore);
		void present_image(uint32_t image_index, VkSemaphore wait_semaphore);

		VkImage get_image(uint32_t image_index) { return this->images[image_index]; };
		VkExtent2D get_image_extent() { return this->image_extent; };
	};
}