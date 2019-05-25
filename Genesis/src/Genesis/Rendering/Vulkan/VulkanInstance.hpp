#pragma once

#include "Genesis/Platform/Window.hpp"

#include "Genesis/Rendering/Vulkan/VulkanInclude.hpp"
#include "Genesis/Rendering/Vulkan/VulkanDebugLayer.hpp"
#include "Genesis/Rendering/Vulkan/VulkanDevice.hpp"
#include "Genesis/Rendering/Vulkan/VulkanSwapchain.hpp"
#include "Genesis/Rendering/Vulkan/VulkanSwapchainFramebuffers.hpp"
#include "Genesis/Rendering/Vulkan/VulkanCommandPool.hpp"

namespace Genesis
{
	class VulkanRenderPipline;

	class VulkanInstance
	{
	public:
		VulkanInstance(Window* window, uint32_t number_of_threads);
		~VulkanInstance();

		bool AcquireSwapchainImage(uint32_t& image_index);
		
		Window* window = nullptr;
		VkInstance instance;
		VkSurfaceKHR surface;

		VulkanDebugLayer* debug_layer = nullptr;
		VulkanDevice* device = nullptr;
		VulkanSwapchain* swapchain = nullptr;
		VulkanSwapchainFramebuffers* swapchain_framebuffers = nullptr;
		VulkanCommandPool* command_pool = nullptr;

		//Allocator
		VmaAllocator allocator;

		//Extensions and Layers
		vector<const char*> getExtensions();
		vector<const char*> getDeviceExtensions();
		vector<const char*> getLayers();

		VkSemaphore image_available_semaphore;
		VkSemaphore render_finished_semaphore;
		VkFence in_flight_fence;

		VkRenderPass screen_render_pass;

		//TEMP DATA
		//Need to abstract later
		VkPipelineLayout colored_mesh_pipeline_layout;
		VulkanRenderPipline* colored_mesh_screen_pipeline = nullptr;
		//END TEMP DATA

	private:
		bool use_debug_layers = true;

		//Instance
		void create_instance(const char* app_name, uint32_t app_version);
		void delete_instance();

		//Surface
		void create_surface(Window* window);
		void delete_surface();

		//TEMP FUNCTIONS
		void create_TEMP();
		void delete_TEMP();
	};

	//TEMP
	struct Vertex
	{
		vector3F pos;
		vector3F normal;
		vector2F texCoord;

		static vector<VkVertexInputBindingDescription> getBindingDescriptions()
		{
			vector<VkVertexInputBindingDescription> bindingDescriptions(1);
			bindingDescriptions[0].binding = 0;
			bindingDescriptions[0].stride = sizeof(Vertex);
			bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

			return bindingDescriptions;
		}

		static vector<VkVertexInputAttributeDescription> getAttributeDescriptions()
		{
			vector<VkVertexInputAttributeDescription> attributeDescriptions(3);

			attributeDescriptions[0].binding = 0;
			attributeDescriptions[0].location = 0;
			attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[0].offset = offsetof(Vertex, pos);

			attributeDescriptions[1].binding = 0;
			attributeDescriptions[1].location = 1;
			attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[1].offset = offsetof(Vertex, normal);

			attributeDescriptions[2].binding = 0;
			attributeDescriptions[2].location = 2;
			attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
			attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

			return attributeDescriptions;
		}
	};
}