#pragma once

#include "Genesis/Graphics/VulkanDevice.hpp"
#include "Genesis/Graphics/VulkanSwapChain.hpp"

namespace Genesis
{
	class VulkanPipelineLayout
	{
	public:
		VulkanPipelineLayout(VulkanDevice* device, VulkanSwapChain* swapChain);
		~VulkanPipelineLayout();

	private:
		VulkanDevice* device = nullptr;

		VkPipelineLayout pipelineLayout;
	};
}