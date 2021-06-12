#pragma once

#include "genesis_core/rendering/rendering_backend.hpp"
#include "genesis_core/rendering/frame_graph.hpp"

#include "vulkan_include.hpp"
#include "device.hpp"
#include "swapchain.hpp"
#include "command_pool.hpp"
#include "bindless_descriptor.hpp"

#include "vulkan_renderer/buffer.hpp"
#include "vulkan_renderer/image.hpp"
#include "vulkan_renderer/mesh_buffer_vector.hpp"

namespace genesis
{
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

	typedef size_t VertexHandle;
	typedef size_t IndexHandle;
	typedef size_t UniformHandle;
	typedef size_t StorageHandle;

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

		MeshBufferVector vertex_buffers;
		MeshBufferVector index_buffers;
		vector<Buffer*> uniform_buffers;

	public:
		Renderer(ApplicationInfo& app_info, Settings& settings, WindowInfo& window);
		~Renderer();

		VertexHandle create_vertex_buffer(size_t buffer_size, MemoryType type);
		void destroy_vertex_buffer(VertexHandle handle);

		IndexHandle create_index_buffer(size_t buffer_size, MemoryType type);
		void destroy_index_buffer(IndexHandle handle);

		UniformHandle create_uniform_buffer(size_t buffer_size, MemoryType type);
		void destroy_uniform_buffer(UniformHandle handle); 

		ShaderModule create_shader_module(void* code_data, size_t code_size);
		void destroy_shader_module(ShaderModule shader);

		void render(genesis::FrameGraph* frame_graph);
	};
}