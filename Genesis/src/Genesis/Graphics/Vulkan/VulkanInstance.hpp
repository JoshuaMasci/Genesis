#pragma once

#include "Genesis/Graphics/GraphicsContext.hpp"
#include "Genesis/Platform/Window.hpp"

#include "Genesis/Graphics/Vulkan/VulkanInclude.hpp"
#include "Genesis/Graphics/Vulkan/VulkanDevice.hpp"
#include "Genesis/Graphics/Vulkan/VulkanSwapchain.hpp"

namespace Genesis
{
	class VulkanInstance
	{
	public:
		VulkanInstance(Window* window);
		~VulkanInstance();

		VkInstance instance;
		VkDebugUtilsMessengerEXT debug_messenger = VK_NULL_HANDLE;
		VkSurfaceKHR surface;

		VulkanDevice* device = nullptr;
		VulkanSwapchain* swapchain = nullptr;

	private:
		bool use_debug_layers = true;

		//Instance
		void create_instance(const char* app_name, uint32_t app_version);
		void delete_instance();

		//Debug
		void create_debug_messenger();
		void delete_debug_messenger();

		//Surface
		void create_surface(Window* window);
		void delete_surface();

		//Extensions and Layers
		vector<const char*> getExtensions();
		vector<const char*> getDeviceExtensions();
		vector<const char*> getLayers();
	};
}