#pragma once

#include "Genesis/Platform/Window.hpp"

#include "Genesis/Rendering/Vulkan/VulkanInclude.hpp"
#include "Genesis/Rendering/Vulkan/VulkanDebugLayer.hpp"
#include "Genesis/Rendering/Vulkan/VulkanDevice.hpp"
#include "Genesis/Rendering/Vulkan/VulkanSwapchain.hpp"
#include "Genesis/Rendering/Vulkan/VulkanSwapchainFramebuffers.hpp"
#include "Genesis/Rendering/Vulkan/VulkanCommandPool.hpp"
#include "Genesis/Rendering/Vulkan/VulkanMultithreadCommandBuffer.hpp"
#include "Genesis/Rendering/Vulkan/VulkanPipelineManager.hpp"
#include "Genesis/Rendering/Vulkan/VulkanDescriptorPool.hpp"
#include "Genesis/Rendering/Vulkan/VulkanFramebuffer.hpp"

#include "Genesis/Rendering/RenderingTypes.hpp"

#include "Genesis/Rendering/Vulkan/VulkanAllocator.hpp"

namespace Genesis
{
	const uint32_t NUM_OF_FRAMES = 3;

	//Texture Stuff: need to move at some point
	struct VulkanTexture
	{
		VkExtent2D size;
		VkImage image;
		VmaAllocation image_memory;
		VmaAllocationInfo image_memory_info;
		VkImageView image_view;

		VkDescriptorSet image_descriptor_set;
	};

	struct VulkanBuffer
	{
		VkBuffer buffer;
		VmaAllocation buffer_memory;
		VmaAllocationInfo buffer_memory_info;

		//Uniform only
		VkDescriptorSet buffer_descriptor_set;
	};

	struct VulkanFrame
	{
		VkSemaphore image_available_semaphore = VK_NULL_HANDLE;

		VulkanMultithreadCommandBuffer* command_buffer = nullptr;
		VkSemaphore command_buffer_done_semaphore = VK_NULL_HANDLE;
		VkFence command_buffer_done_fence = VK_NULL_HANDLE;

		//Uniform Objects here

		//Frame Buffers here
	};

	class VulkanInstance
	{
	public:
		VulkanInstance(Window* window, uint32_t number_of_threads);
		~VulkanInstance();

		bool AcquireSwapchainImage(uint32_t& image_index, VkSemaphore signal_semaphore);
		
		Window* window = nullptr;
		VkInstance instance;
		VkSurfaceKHR surface;

		VulkanDebugLayer* debug_layer = nullptr;
		VulkanDevice* device = nullptr;
		VulkanAllocator* allocator = nullptr;
		VulkanSwapchain* swapchain = nullptr;
		VulkanSwapchainFramebuffers* swapchain_framebuffers = nullptr;

		VulkanCommandPool* primary_command_pool = nullptr;
		Array<VulkanCommandPool*> secondary_command_pools;

		Array<VulkanFrame> frames_in_flight;

		VulkanPiplineManager* pipeline_manager = nullptr;

		VulkanDescriptorPool* descriptor_pool = nullptr;

		VulkanFramebufferLayout* shadow_pass_layout = nullptr;

		//Extensions and Layers
		vector<const char*> getExtensions();
		vector<const char*> getDeviceExtensions();
		vector<const char*> getLayers();

		//Texture Stuff: need to move at some point
		TextureIndex next_index_texture = 1;
		map<TextureIndex, VulkanTexture> textures;
		VkSampler linear_sampler;

		//Buffer Stuff
		BufferIndex next_index_buffer = 1;
		map<BufferIndex, VulkanBuffer> buffers;

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