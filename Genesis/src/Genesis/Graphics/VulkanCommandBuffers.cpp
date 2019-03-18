#include "VulkanCommandBuffers.hpp"

#include "Genesis/Graphics/VulkanQueueFamilyIndices.hpp"

using namespace Genesis;

VulkanCommandBuffers::VulkanCommandBuffers(VulkanDevice* device, VulkanSwapChainFramebuffer* framebuffers, VulkanSwapChain* swapChain_temp, VulkanPipeline* pipeline_temp)
{
	this->device = device;

	VulkanQueueFamilyIndices indices = VulkanQueueFamilyIndices::findQueueFamilies(this->device->getPhysicalDevice(), nullptr);

	VkCommandPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = indices.graphicsFamily.value();
	poolInfo.flags = 0;

	if (vkCreateCommandPool(this->device->getDevice(), &poolInfo, nullptr, &this->commandPool) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create command pool!");
	}

	this->commandBuffers.resize(framebuffers->getFramebuffers().size());

	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

	if (vkAllocateCommandBuffers(this->device->getDevice(), &allocInfo, this->commandBuffers.data()) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate command buffers!");
	}

	for (size_t i = 0; i < this->commandBuffers.size(); i++) 
	{
		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

		if (vkBeginCommandBuffer(this->commandBuffers[i], &beginInfo) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to begin recording command buffer!");
		}

		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = pipeline_temp->getRenderPass()->getRenderPass();
		renderPassInfo.framebuffer = framebuffers->getFramebuffers()[i];
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = swapChain_temp->getSwapChainExtent();

		VkClearValue clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
		renderPassInfo.clearValueCount = 1;
		renderPassInfo.pClearValues = &clearColor;

		vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_temp->getPipeline());

		vkCmdDraw(commandBuffers[i], 3, 1, 0, 0);

		vkCmdEndRenderPass(commandBuffers[i]);

		if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) 
		{
			throw std::runtime_error("failed to record command buffer!");
		}
	}
}

VulkanCommandBuffers::~VulkanCommandBuffers()
{
	vkDestroyCommandPool(this->device->getDevice(), this->commandPool, nullptr);
}

vector<VkCommandBuffer> Genesis::VulkanCommandBuffers::getCommandBuffers()
{
	return this->commandBuffers;
}
