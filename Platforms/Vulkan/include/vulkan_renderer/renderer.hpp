#pragma once

#include "genesis_core/rendering/rendering_backend.hpp"

#include "vulkan_include.hpp"
#include "device.hpp"
#include "swapchain.hpp"
#include "command_pool.hpp"
#include "bindless_descriptor.hpp"

namespace genesis
{
	struct ApplicationInfo
	{
		char* app_name;
		uint32_t app_version;
	};

	struct Settings
	{
		uint32_t max_uniform_buffer_bindings;
		uint32_t max_storage_buffer_bindings;
		uint32_t max_sampled_image_bindings;
		uint32_t max_storage_image_bindings;
	};

	struct WindowInfo
	{
		void* handle;
		uint32_t width;
		uint32_t height;
	};

	class DebugLayer
	{
	public:
		void create(VkInstance instance);
		void destroy();

	private:
		VkInstance instance;
		VkDebugUtilsMessengerEXT debug_messenger = VK_NULL_HANDLE;
	};

	class Renderer
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

		unique_ptr<CommandPool> primary_command_pool;
		unique_ptr<BindlessDescriptor> bindless_descriptor;

	public:
		Renderer(ApplicationInfo& app_info, Settings& settings, WindowInfo& window);
		~Renderer();

		BufferHandle create_buffer(size_t buffer_size, Temp_BufferUsage usage, Temp_MemoryType type);
		void destoy_buffer(BufferHandle buffer);

		void render(genesis::FrameGraph* frame_graph);
	};
}