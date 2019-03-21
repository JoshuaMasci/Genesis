#pragma once

#include "Genesis/Core/Types.hpp"
#include "Genesis/Platform/Window.hpp"

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

namespace Genesis
{
	class Renderer {};//TEMP, will move later

	struct VulkanDeviceProperties
	{
		VkPhysicalDevice physical_device;
		VkPhysicalDeviceProperties properties;
		VkPhysicalDeviceMemoryProperties memory_properties;
		VkPhysicalDeviceFeatures features;
		VkSurfaceCapabilitiesKHR surface_capabilities;
	};

	struct SwapchainProperties
	{
		VkFormat swapchain_image_format;
		VkExtent2D swapchain_extent;
	};

	struct SwapchainImages
	{
		vector<VkImage> swapchain_images;
		vector<VkImageView> swapchain_imageviews;
		vector<VkFramebuffer> swapchain_framebuffers;
	};

	struct VulkanContext
	{
		//Instance
		VkInstance instance;

		//Debug
		VkDebugUtilsMessengerEXT debug_messenger = VK_NULL_HANDLE;

		//Surface
		VkSurfaceKHR surface;

		//Device and Queues
		VkDevice device;
		VulkanDeviceProperties device_properties;
		VkQueue graphics_queue;
		VkQueue present_queue;

		//Swapchain
		VkSwapchainKHR swapchain;
		SwapchainProperties swapchain_properties;
		SwapchainImages swapchain_images;
	};

	class VulkanRenderer : public Renderer
	{
	public:
		VulkanRenderer(Window* window, const char* app_name);
		virtual ~VulkanRenderer();

	private:
		Window* window = nullptr;
		VulkanContext context;

		bool use_debug_layers = true;

		//Instance
		void create_instance(const char* app_name, uint32_t app_version);
		void delete_instance();

		//Debug
		void create_debug_messenger();
		void delete_debug_messenger();

		//Surface
		void create_surface();
		void delete_surface();

		//Device
		void create_device_and_queues(VkPhysicalDevice physical_device);
		void delete_device_and_queues();

		//Extensions and Layers
		vector<const char*> getExtensions();
		vector<const char*> getDeviceExtensions();
		vector<const char*> getLayers();
	};
}