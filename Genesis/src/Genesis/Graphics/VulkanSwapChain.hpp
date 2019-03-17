#pragma once

#include "Genesis/Graphics/VulkanDevice.hpp"
#include "Genesis/Graphics/VulkanSurface.hpp"
#include "Genesis/Platform/Window.hpp"

namespace Genesis
{
	class VulkanSwapChain
	{
	public:
		VulkanSwapChain(VulkanDevice* device, VulkanSurface* surface, Window* window);
		~VulkanSwapChain();

		void recreateSwapChain(Window* window) {};

		VkSwapchainKHR getSwapChain();

		vector<VkImage> getSwapChainImages();
		VkFormat getSwapChainImageFormat();

		VkExtent2D getSwapChainExtent();

	private:
		VulkanDevice* device = nullptr;

		VkSwapchainKHR swapChain;

		VkFormat swapChainImageFormat;
		VkExtent2D swapChainExtent;

		vector<VkImage> swapChainImages;
	};
}