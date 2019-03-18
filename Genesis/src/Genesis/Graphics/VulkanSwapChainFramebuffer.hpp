#pragma once

#include "Genesis/Graphics/VulkanDevice.hpp"
#include "Genesis/Graphics/VulkanSwapChain.hpp"
#include "Genesis/Graphics/VulkanRenderPass.hpp"

namespace Genesis
{
	class VulkanSwapChainFramebuffer
	{
	public:
		VulkanSwapChainFramebuffer(VulkanDevice* device, VulkanSwapChain* swapChain, VulkanRenderPass* renderPass);
		~VulkanSwapChainFramebuffer();

		vector<VkFramebuffer> getFramebuffers();

	private:
		VulkanDevice* device = nullptr;

		vector<VkFramebuffer> swapChainFramebuffers;
	};
}