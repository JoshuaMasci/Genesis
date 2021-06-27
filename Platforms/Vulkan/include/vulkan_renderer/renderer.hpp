#pragma once

#include "vulkan_include.hpp"
#include "device.hpp"
#include "swapchain.hpp"
#include "command_pool.hpp"
#include "bindless_descriptor.hpp"

namespace genesis
{
	enum class MemoryType
	{
		GpuOnly,
		CpuRead,
		CpuWrite,
	};

	enum class ImageFormat
	{
		Invalid,

		RGBA_8_Unorm,

		R_16_Float,
		RG_16_Float,
		RGB_16_Float,
		RGBA_16_Float,

		R_32_Float,
		RG_32_Float,
		RGB_32_Float,
		RGBA_32_Float,

		//Depth Images
		D_16_Unorm,
		D_32_Float,
	};

	struct ImageCreateInfo
	{
		vec2u size;
		ImageFormat format;
		uint16_t sampele_count = 1;
		bool optimal_tiling = true;
		MemoryType memory_type = MemoryType::GpuOnly;
	};

	struct ApplicationInfo
	{
		char* app_name;
		uint32_t app_version;
	};

	struct Settings
	{
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

		size_t create_image(const ImageCreateInfo& create_info);
		void destroy_image(size_t id);

		void draw_frame();
	};
}