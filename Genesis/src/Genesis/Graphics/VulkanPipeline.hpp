#pragma once

#include "Genesis/Graphics/VulkanDevice.hpp"
#include "Genesis/Graphics/VulkanSwapChain.hpp"

#include "Genesis/Graphics/VulkanShader.hpp"
#include "Genesis/Graphics/VulkanRenderPass.hpp"

namespace Genesis
{
	class VulkanPipeline
	{
	public:
		VulkanPipeline(VulkanDevice* device, VulkanSwapChain* swapChain);
		~VulkanPipeline();

		VkPipeline getPipeline();
		VulkanRenderPass* getRenderPass();

	private:
		VulkanDevice* device = nullptr;
		VulkanRenderPass* renderPass = nullptr;

		VkPipeline pipeline;
		VkPipelineLayout pipelineLayout;
	};
}