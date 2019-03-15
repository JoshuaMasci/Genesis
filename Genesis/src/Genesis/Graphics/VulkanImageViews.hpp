#pragma once

#include "Genesis/Graphics/VulkanDevice.hpp"
#include "Genesis/Graphics/VulkanSwapChain.hpp"

namespace Genesis
{
	class VulkanImageViews
	{
	public:
		VulkanImageViews(VulkanDevice* device, VulkanSwapChain* surface);
		~VulkanImageViews();

	private:
		VulkanDevice* device = nullptr;

		std::vector<VkImageView> swapChainImageViews;
	};
}