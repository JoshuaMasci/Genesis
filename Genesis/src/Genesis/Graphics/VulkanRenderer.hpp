#pragma once

#include "Genesis/Core/Types.hpp"
#include "Genesis/Platform/Window.hpp"

#define NOMINMAX
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "vk_mem_alloc.h"

#include <array>

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

		VkImage depth_image;
		VkImageView depth_imageview;
		VmaAllocation depth_image_memory;
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

		//Allocator
		VmaAllocator allocator;

		//Swapchain
		VkSwapchainKHR swapchain;
		uint32_t swapchain_image_count;
		SwapchainProperties swapchain_properties;
		SwapchainImages swapchain_images;

		//Screen Render Pass
		VkRenderPass screen_render_pass;
		
		//Command Buffers
		VkCommandPool command_pool;
		vector<VkCommandBuffer> command_buffers;

		//Sync Objects
		vector<VkSemaphore> image_available_semaphores;
		vector<VkSemaphore> render_finished_semaphores;
		vector<VkFence> in_flight_fences;
	};

	class VulkanRenderer : public Renderer
	{
	public:
		VulkanRenderer(Window* window, const char* app_name);
		virtual ~VulkanRenderer();

		void render(double delta_time);

	private:
		Window* window = nullptr;
		VulkanContext context;

		//Temp Pipeline Stuff
		VkPipelineLayout pipeline_layout;
		VkPipeline pipeline;
		void create_pipeline();
		void delete_pipeline();

		VkDescriptorSetLayout descriptor_set_layout;
		void create_descriptor_set_layout();
		void delete_descriptor_set_layout();

		vector<VkBuffer> uniform_buffers;
		vector<VmaAllocation> uniform_buffers_memory;
		void create_uniform_buffer();
		void delete_uniform_buffer();
		void update_uniform_buffer(uint32_t index, matrix4F matrix);

		VkDescriptorPool descriptor_pool;
		vector<VkDescriptorSet> descriptor_sets;
		void create_descriptor_set();
		void delete_descriptor_set();

		//More Temp Stuff
		VkBuffer vertex_buffer;
		VmaAllocation vertex_buffer_memory;
		void create_cube_buffer();
		void delete_cube_buffer();

		void buildCommandBuffer(uint32_t index);

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

		//Allocator
		void create_allocator();
		void delete_allocator();

		//Swapchain
		void create_swapchain();
		void delete_swapchain();

		//Screen Render Pass
		void create_screen_render_pass();
		void delete_screen_render_pass();

		//Swapchain Framebuffers
		void create_swapchain_framebuffers();
		void delete_swapchain_framebuffers();

		//Command Buffers
		void create_command_buffers();
		void delete_command_buffers();

		//Sync Objects
		void create_sync_objects();
		void delete_sync_objects();

		//Extensions and Layers
		vector<const char*> getExtensions();
		vector<const char*> getDeviceExtensions();
		vector<const char*> getLayers();
	};


	//TEMP CODE
	struct UniformMatrices
	{
		matrix4F ModelViewProj;
	};

	struct Vertex 
	{
		vector3F pos;
		vector3F color;
		vector2F texCoord;

		static VkVertexInputBindingDescription getBindingDescription() 
		{
			VkVertexInputBindingDescription bindingDescription = {};
			bindingDescription.binding = 0;
			bindingDescription.stride = sizeof(Vertex);
			bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

			return bindingDescription;
		}

		static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() 
		{
			std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions = {};

			attributeDescriptions[0].binding = 0;
			attributeDescriptions[0].location = 0;
			attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[0].offset = offsetof(Vertex, pos);

			attributeDescriptions[1].binding = 0;
			attributeDescriptions[1].location = 1;
			attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[1].offset = offsetof(Vertex, color);

			/*attributeDescriptions[2].binding = 0;
			attributeDescriptions[2].location = 2;
			attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
			attributeDescriptions[2].offset = offsetof(Vertex, texCoord);*/

			return attributeDescriptions;
		}
	};

}