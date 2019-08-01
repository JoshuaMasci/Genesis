#pragma once

#include "Genesis/Platform/Window.hpp"

#include "Genesis/Rendering/Vulkan/VulkanInclude.hpp"
#include "Genesis/Rendering/Vulkan/VulkanDebugLayer.hpp"
#include "Genesis/Rendering/Vulkan/VulkanDevice.hpp"
#include "Genesis/Rendering/Vulkan/VulkanAllocator.hpp"
#include "Genesis/Rendering/Vulkan/VulkanSwapchain.hpp"
#include "Genesis/Rendering/Vulkan/VulkanSwapchainFramebuffers.hpp"
#include "Genesis/Rendering/Vulkan/VulkanCommandPool.hpp"
#include "Genesis/Rendering/Vulkan/VulkanMultithreadCommandBuffer.hpp"
#include "Genesis/Rendering/Vulkan/VulkanDescriptorPool.hpp"
#include "Genesis/Rendering/Vulkan/VulkanFramebuffer.hpp"
#include "Genesis/Rendering/Vulkan/VulkanShader.hpp"
#include "Genesis/Rendering/Vulkan/VulkanBuffer.hpp"
#include "Genesis/Rendering/Vulkan/VulkanTexture.hpp"
#include "Genesis/Rendering/Vulkan/VulkanView.hpp"

#include "Genesis/Rendering/DelayedResourceDeleter.hpp"

namespace Genesis
{
	const uint32_t NUM_OF_FRAMES = 3;
	struct VulkanFrame
	{
		VkSemaphore image_available_semaphore = VK_NULL_HANDLE;
		VulkanMultithreadCommandBuffer* command_buffer = nullptr;
		VkSemaphore command_buffer_done_semaphore = VK_NULL_HANDLE;
		VkFence command_buffer_done_fence = VK_NULL_HANDLE;
	};

	class VulkanInstance
	{
	public:
		VulkanInstance(Window* window, uint32_t number_of_threads);
		~VulkanInstance();

		bool acquireSwapchainImage(uint32_t& image_index, VkSemaphore signal_semaphore);
		
		Window* window = nullptr;
		VkInstance instance;
		VkSurfaceKHR surface;

		VulkanDebugLayer* debug_layer = nullptr;
		VulkanDevice* device = nullptr;
		VulkanAllocator* allocator = nullptr;
		VulkanSwapchain* swapchain = nullptr;
		VulkanSwapchainFramebuffers* swapchain_framebuffers = nullptr;

		VulkanCommandPoolSet* graphics_command_pool_set = nullptr;

		Array<VulkanFrame> frames_in_flight;

		VulkanDescriptorPool* image_descriptor_pool = nullptr;
		VulkanDescriptorPool* uniform_descriptor_pool = nullptr;

		VulkanFramebufferLayout* shadow_pass_layout = nullptr;
		VulkanFramebufferLayout* color_pass_layout = nullptr;

		//Extensions and Layers
		vector<const char*> getExtensions();
		vector<const char*> getDeviceExtensions();
		vector<const char*> getLayers();

		VkSampler linear_sampler;

		//Resource Stuff
		void cycleResourceDeleters();
		DelayedResourceDeleter<VulkanBuffer>* buffer_deleter = nullptr;
		DelayedResourceDeleter<VulkanTexture>* texture_deleter = nullptr;
		DelayedResourceDeleter<VulkanView>* view_deleter = nullptr;
		DelayedResourceDeleter<VulkanShader>* shader_deleter = nullptr;

	private:
		bool use_debug_layers = true;

		//Instance
		void create_instance(const char* app_name, uint32_t app_version);
		void delete_instance();

		//Surface
		void create_surface(Window* window);
		void delete_surface();
	};
}