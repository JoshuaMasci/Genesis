#pragma once

#include "Genesis/Core/Types.hpp"

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "Genesis/Graphics/VulkanInstance.hpp"
#include "Genesis/Graphics/VulkanSurface.hpp"
#include "Genesis/Graphics/VulkanDebugMessenger.hpp"
#include "Genesis/Graphics/VulkanDevice.hpp"
#include "Genesis/Graphics/VulkanSwapChain.hpp"
#include "Genesis/Graphics/VulkanShader.hpp"
#include "Genesis/Graphics/VulkanPipeline.hpp"
#include "Genesis/Graphics/VulkanSwapChainFramebuffer.hpp"
#include "Genesis/Graphics/VulkanCommandBuffers.hpp"

namespace Genesis
{
	//Prototype
	class Window;

	class VulkanGraphics
	{
	public:
		VulkanGraphics(Window* window);
		~VulkanGraphics();

		void render();

	private:
		Window* const window;

		VulkanConfig config;
		VulkanInstance* instance = nullptr;
		VulkanSurface* surface = nullptr;
		VulkanDebugMessenger* debugMessenger = nullptr;
		VulkanDevice* device = nullptr;
		VulkanSwapChain* swapChain = nullptr;
		VulkanPipeline* pipeline = nullptr;
		VulkanSwapChainFramebuffer* framebuffers = nullptr;
		VulkanCommandBuffers* commandBuffers = nullptr;

		VkSemaphore imageAvailableSemaphore;
		VkSemaphore renderFinishedSemaphore;

		bool debug_validation_layers = true;

		void initVulkan();
		void destroyVulkan();

		vector<const char*> getRequiredExtensions();
		vector<const char*> getRequiredLayers();
	};
}