#pragma once

#include "Genesis/Graphics/VulkanDevice.hpp"
#include "Genesis/Graphics/VulkanSwapChainFramebuffer.hpp"

#include "Genesis/Graphics/VulkanPipeline.hpp"

namespace Genesis
{
	class VulkanCommandBuffers
	{
	public:
		VulkanCommandBuffers(VulkanDevice* device, VulkanSwapChainFramebuffer* framebuffers, VulkanSwapChain* swapChain_temp, VulkanPipeline* pipeline_temp);
		~VulkanCommandBuffers();

		vector<VkCommandBuffer> getCommandBuffers();

	private:
		VulkanDevice* device = nullptr;

		VkCommandPool commandPool;

		vector<VkCommandBuffer> commandBuffers;
	};
}