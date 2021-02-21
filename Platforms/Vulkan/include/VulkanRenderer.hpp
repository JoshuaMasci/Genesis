#pragma once

#include "VulkanInclude.hpp"
#include "Device.hpp"
#include "Swapchain.hpp"
#include "CommandPool.hpp"
#include <string>
#include <vector>
#include <unordered_map>

namespace VulkanRenderer
{
	struct VulkanApplication
	{
		char* app_name;
		uint32_t app_version;
	};

	struct VulkanSettings
	{
	};

	struct VulkanWindow
	{
		void* handle;
		uint32_t width;
		uint32_t height;
	};

	class FrameGraph;

	class DebugLayer
	{
	public:
		void create(VkInstance instance);
		void destroy();

	private:
		VkInstance instance;
		VkDebugUtilsMessengerEXT debug_messenger = VK_NULL_HANDLE;
	};

	class VulkanRenderer
	{
	protected:
		VkInstance instance = VK_NULL_HANDLE;
		DebugLayer debug_layer;
		VkSurfaceKHR surface = VK_NULL_HANDLE;

		Device* device = nullptr;
		Swapchain* swapchain = nullptr;

		VkFence wait_fence = VK_NULL_HANDLE;
		VkSemaphore image_wait_semaphore = VK_NULL_HANDLE;
		VkSemaphore present_wait_semaphore = VK_NULL_HANDLE;

		CommandPool* primary_command_pool = nullptr;

	public:
		VulkanRenderer(VulkanApplication& app, VulkanSettings& settings, VulkanWindow& window);
		~VulkanRenderer();

		void render(FrameGraph* frame_graph);
	};
}