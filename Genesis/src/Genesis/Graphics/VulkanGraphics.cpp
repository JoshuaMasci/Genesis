#include "VulkanGraphics.hpp"

#include "Genesis/Platform/Window.hpp"
#include "Genesis/Graphics/VulkanPhysicalDevicePicker.hpp"

#include <limits>

using namespace Genesis;

VulkanGraphics::VulkanGraphics(Window* window)
	:window(window)
{
	this->initVulkan();
}

VulkanGraphics::~VulkanGraphics()
{
	this->destroyVulkan();
}

void VulkanGraphics::render()
{
	uint32_t imageIndex;
	vkAcquireNextImageKHR(this->device->getDevice(), this->swapChain->getSwapChain(), INT64_MAX, imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { imageAvailableSemaphore };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;

	vector<VkCommandBuffer> commandBuffers = this->commandBuffers->getCommandBuffers();
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffers[imageIndex];

	VkSemaphore signalSemaphores[] = { renderFinishedSemaphore };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	if (vkQueueSubmit(this->device->getGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS) 
	{
		throw std::runtime_error("failed to submit draw command buffer!");
	}

	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;
	VkSwapchainKHR swapChains[] = { this->swapChain->getSwapChain() };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;
	presentInfo.pResults = nullptr; // Optional

	vkQueuePresentKHR(this->device->getPresentQueue(), &presentInfo);
}

void VulkanGraphics::initVulkan()
{
	this->config.setRequiredExtensions(this->getRequiredExtensions());
	this->config.setRequiredDeviceExtensions({ VK_KHR_SWAPCHAIN_EXTENSION_NAME });
	this->config.setRequiredLayers(this->getRequiredLayers());

	this->instance = new VulkanInstance(this->config, "Sandbox", VK_MAKE_VERSION(0, 0, 0));
	this->surface = new VulkanSurface(this->instance, this->window);

	if (this->debug_validation_layers)
	{
		this->debugMessenger = new VulkanDebugMessenger(this->instance);
	}

	VulkanPhysicalDevicePicker picker = VulkanPhysicalDevicePicker(this->instance, this->surface);
	this->device = new VulkanDevice(this->config, this->surface, picker.pickDevice());

	this->swapChain = new VulkanSwapChain(this->device, this->surface, this->window);

	this->pipeline = new VulkanPipeline(this->device, this->swapChain);

	this->framebuffers = new VulkanSwapChainFramebuffer(this->device, this->swapChain, this->pipeline->getRenderPass());

	this->commandBuffers = new VulkanCommandBuffers(this->device, this->framebuffers, this->swapChain, this->pipeline);

	VkSemaphoreCreateInfo semaphoreInfo = {};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	if (vkCreateSemaphore(this->device->getDevice(), &semaphoreInfo, nullptr, &imageAvailableSemaphore) != VK_SUCCESS ||
		vkCreateSemaphore(this->device->getDevice(), &semaphoreInfo, nullptr, &renderFinishedSemaphore) != VK_SUCCESS) 
	{

		throw std::runtime_error("failed to create semaphores!");
	}
}

void VulkanGraphics::destroyVulkan()
{
	vkDestroySemaphore(this->device->getDevice(), renderFinishedSemaphore, nullptr);
	vkDestroySemaphore(this->device->getDevice(), imageAvailableSemaphore, nullptr);

	delete this->commandBuffers;

	delete this->framebuffers;

	delete this->pipeline;

	delete this->swapChain;
	delete this->device;

	if (this->debug_validation_layers)
	{
		delete this->debugMessenger;
	}

	delete this->surface;
	delete this->instance;
}

vector<const char*> VulkanGraphics::getRequiredExtensions()
{
	vector<const char*> extensions;

	extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
	extensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);

	if (this->debug_validation_layers)
	{
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
	}

	return extensions;
}

vector<const char*> VulkanGraphics::getRequiredLayers()
{
	vector<const char*> layers;

	if (this->debug_validation_layers)
	{
		layers.push_back("VK_LAYER_LUNARG_standard_validation");
	}

	return layers;
}