#include "vulkan_renderer/swapchain.hpp"

namespace genesis
{
	uint32_t clamp(uint32_t value, uint32_t min, uint32_t max)
	{
		if (value < min)
		{
			return min;
		}
		else if(value > max)
		{
			return max;
		}

		return value;
	}

	VkSwapchainKHR Swapchain::create_swapchain(VkExtent2D surface_size)
	{
		VkSurfaceCapabilitiesKHR capabilities;
		VK_ASSERT(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(this->device->get_physical_device(), surface, &capabilities));

		VkSwapchainCreateInfoKHR create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		create_info.surface = this->surface;
		create_info.oldSwapchain = this->swapchain;

		{
			uint32_t image_count = capabilities.minImageCount + 1;
			if (capabilities.maxImageCount > 0 && image_count > capabilities.maxImageCount)
			{
				image_count = capabilities.maxImageCount;
			}
			create_info.minImageCount = image_count;
		}

		{
			//Force this format
			//TODO: decide surface format
			VkSurfaceFormatKHR format = { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
			create_info.imageFormat = format.format;
			create_info.imageColorSpace = format.colorSpace;
		}

		{
			VkExtent2D swapchain_size = surface_size;
			if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
			{
				swapchain_size = capabilities.currentExtent;
			}
			else
			{
				VkExtent2D actual_extent = swapchain_size;
				actual_extent.width = clamp(actual_extent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
				actual_extent.height = clamp(actual_extent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
				swapchain_size = actual_extent;
			}

			this->image_extent = create_info.imageExtent = swapchain_size;
		}

		{
			std::vector<VkPresentModeKHR> present_modes;
			VkPresentModeKHR best_mode = VK_PRESENT_MODE_FIFO_KHR; //All systesm support this one
			uint32_t present_count = 0;
			vkGetPhysicalDeviceSurfacePresentModesKHR(this->device->get_physical_device(), surface, &present_count, nullptr);
			if (present_count != 0)
			{
				present_modes.resize(present_count);
				vkGetPhysicalDeviceSurfacePresentModesKHR(this->device->get_physical_device(), surface, &present_count, present_modes.data());
				for (const auto& availablePresentMode : present_modes)
				{
					if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
					{
						best_mode = availablePresentMode; //The mode we want
						break;
					}
				}
			}
			create_info.presentMode = best_mode;
		}

		{
			//Present wueue is the same and Graphics queue right now
			create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			create_info.queueFamilyIndexCount = 0;
			create_info.pQueueFamilyIndices = nullptr;
		}

		//Werid stuff, should lookup what it does
		create_info.preTransform = capabilities.currentTransform;
		create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		create_info.clipped = VK_TRUE;

		create_info.imageArrayLayers = 1;
		create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		VkSwapchainKHR new_swapchain = VK_NULL_HANDLE;
		VK_ASSERT(vkCreateSwapchainKHR(device->get(), &create_info, nullptr, &new_swapchain));

		uint32_t image_count;
		vkGetSwapchainImagesKHR(this->device->get(), new_swapchain, &image_count, nullptr);
		this->images.resize(image_count);
		vkGetSwapchainImagesKHR(device->get(), new_swapchain, &image_count, this->images.data());

		return new_swapchain;
	}

	Swapchain::Swapchain(Device* device, VkSurfaceKHR surface, VkExtent2D surface_size)
	{
		this->device = device;
		this->surface = surface;
		this->swapchain = this->create_swapchain(surface_size);
	}

	Swapchain::~Swapchain()
	{
		vkDestroySwapchainKHR(this->device->get(), this->swapchain, nullptr);
	}

	uint32_t Swapchain::get_next_image(VkSemaphore image_ready_semaphore)
	{
		uint32_t image_index;
		while (vkAcquireNextImageKHR(this->device->get(), this->swapchain, std::numeric_limits<uint64_t>::max(), image_ready_semaphore, VK_NULL_HANDLE, &image_index) != VK_SUCCESS)
		{
			VkSurfaceCapabilitiesKHR capabilities;
			VK_ASSERT(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(this->device->get_physical_device(), surface, &capabilities));
			this->swapchain = this->create_swapchain(capabilities.currentExtent);
		}

		return image_index;
	}

	void Swapchain::present_image(uint32_t image_index, VkSemaphore wait_semaphore)
	{
		VkPresentInfoKHR present_info = {};
		present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

		if (wait_semaphore != VK_NULL_HANDLE)
		{
			present_info.waitSemaphoreCount = 1;
			present_info.pWaitSemaphores = &wait_semaphore;
		}
		else
		{
			present_info.waitSemaphoreCount = 0;
		}

		VkSwapchainKHR swapchains[] = { this->swapchain };
		present_info.swapchainCount = 1;
		present_info.pSwapchains = swapchains;
		present_info.pImageIndices = &image_index;

		VK_ASSERT(vkQueuePresentKHR(this->device->get_queue(), &present_info));
	}
}