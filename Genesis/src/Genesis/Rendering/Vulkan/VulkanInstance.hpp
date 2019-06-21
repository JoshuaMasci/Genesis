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

#include "Genesis/Rendering/RenderingTypes.hpp"

namespace Genesis
{
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


	struct VulkanShadowMapImage
	{
		VkImage depth_image;
		VmaAllocation depth_image_memory;
		VkImageView depth_image_view;
		VkDescriptorSet depth_image_descriptor_set;

		VkFramebuffer depth_frame_buffer;
	};

	struct VulkanShadowMap
	{
		VkExtent2D size;
		Array<VulkanShadowMapImage> images;
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
		VulkanSwapchain* swapchain = nullptr;
		VulkanSwapchainFramebuffers* swapchain_framebuffers = nullptr;

		VulkanCommandPool* primary_command_pool = nullptr;
		Array<VulkanCommandPool*> secondary_command_pools;

		Array<VulkanFrame> frames_in_flight;

		//Allocator
		VmaAllocator allocator;

		VulkanPiplineManager* pipeline_manager = nullptr;

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

		VkDescriptorPool descriptor_pool;

		//Shadow Map Stuff
		VkRenderPass shadow_render_pass;
		void buildShadowRenderPass(VkFormat depth_format);
		VkPipelineLayout shadow_pipeline_layout;

		ShadowMapIndex next_index_shadow_map = 1;
		map<ShadowMapIndex, VulkanShadowMap> shadow_maps;

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