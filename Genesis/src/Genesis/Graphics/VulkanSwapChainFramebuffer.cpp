#include "VulkanSwapChainFramebuffer.hpp"

using namespace Genesis;

VulkanSwapChainFramebuffer::VulkanSwapChainFramebuffer(VulkanDevice* device, VulkanSwapChain* swapChain, VulkanRenderPass* renderPass)
{
	this->device = device;
	
	auto swapChainImageViews = swapChain->getSwapChainImageViews();
	VkExtent2D swapChainExtent = swapChain->getSwapChainExtent();


	this->swapChainFramebuffers.resize(swapChainImageViews.size());

	for (size_t i = 0; i < swapChainImageViews.size(); i++) 
	{
		VkImageView attachments[] = 
		{
			swapChainImageViews[i]
		};

		VkFramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = renderPass->getRenderPass();
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.pAttachments = attachments;
		framebufferInfo.width = swapChainExtent.width;
		framebufferInfo.height = swapChainExtent.height;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(this->device->getDevice(), &framebufferInfo, nullptr, &this->swapChainFramebuffers[i]) != VK_SUCCESS) 
		{
			throw std::runtime_error("failed to create framebuffer!");
		}
	}
}

VulkanSwapChainFramebuffer::~VulkanSwapChainFramebuffer()
{
	for (auto framebuffer : this->swapChainFramebuffers) 
	{
		vkDestroyFramebuffer(this->device->getDevice(), framebuffer, nullptr);
	}
}

vector<VkFramebuffer> VulkanSwapChainFramebuffer::getFramebuffers()
{
	return this->swapChainFramebuffers;
}
