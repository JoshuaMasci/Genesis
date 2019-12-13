#pragma once

#include "Genesis/Rendering/RenderingBackend.hpp"
#include "Genesis/Platform/Window.hpp"

#include "Genesis/Rendering/Vulkan/VulkanInclude.hpp"
#include "Genesis/Rendering/Vulkan/VulkanInstance.hpp"
#include "Genesis/Rendering/Vulkan/VulkanDebugLayer.hpp"
#include "Genesis/Rendering/Vulkan/VulkanDevice.hpp"
#include "Genesis/Rendering/Vulkan/VulkanSwapchain.hpp"

#include "Genesis/Rendering/Vulkan/VulkanCommandPool.hpp"
#include "Genesis/Rendering/Vulkan/VulkanDescriptorPool.hpp"
#include "Genesis/Rendering/Vulkan/VulkanPipelinePool.hpp"
#include "Genesis/Rendering/Vulkan/VulkanRenderPassPool.hpp"
#include "Genesis/Rendering/Vulkan/VulkanTransferBuffer.hpp"

namespace Genesis
{
	class VulkanBackend : public RenderingBackend
	{
	public:
		VulkanBackend(Window* window, uint32_t number_of_threads);
		~VulkanBackend();

		virtual void setScreenSize(vector2U size) override;
		virtual vector2U getScreenSize() override;

		virtual bool beginFrame() override;
		virtual void endFrame() override;

		virtual VertexBuffer createVertexBuffer(void* data, uint64_t data_size, VertexInputDescription& vertex_input_description, MemoryUsage memory_usage = MemoryUsage::GPU_Only) override;
		virtual void destroyVertexBuffer(VertexBuffer vertex_buffer_index) override;

		virtual IndexBuffer createIndexBuffer(void* data, uint64_t data_size, IndexType type, MemoryUsage memory_usage = MemoryUsage::GPU_Only) override;
		virtual void destroyIndexBuffer(IndexBuffer index_buffer_index) override;

		virtual UniformBuffer createUniformBuffer(uint64_t data_size, MemoryUsage memory_usage = MemoryUsage::CPU_Visable) override;
		virtual void destroyUniformBuffer(UniformBuffer* uniform_buffer) override;

		virtual Texture createTexture(vector2U size, void* data, uint64_t data_size) override;
		virtual void destroyTexture(Texture texture_handle) override;

		virtual Shader createShader(string& vert_data, string& frag_data) override;
		virtual void destroyShader(Shader shader_handle) override;

		virtual void waitTillDone() override;

	private:
		const uint32_t FRAME_COUNT;
		const uint32_t THREAD_COUNT;

		Window* window = nullptr;

		VkInstance instance = VK_NULL_HANDLE;
		VkSurfaceKHR surface = VK_NULL_HANDLE;

		VulkanDebugLayer* debug_layer = nullptr;

		VulkanDevice* device = nullptr;
		VulkanSwapchain* swapchain = nullptr;

		uint32_t swapchain_image_index = 0;
		uint32_t frame_index = 0;

		struct Frame
		{
			VkCommandBuffer screen_command_buffer = VK_NULL_HANDLE;

			VkSemaphore image_ready_semaphore = VK_NULL_HANDLE;
			VkSemaphore command_buffer_done_semaphore = VK_NULL_HANDLE;
			VkFence frame_done_fence = VK_NULL_HANDLE;

			VulkanTransferBuffer* transfer_buffer = nullptr;
		};
		List<Frame> frames;

		//Graphics Command Pools
		VulkanCommandPool* primary_graphics_pool = nullptr;
		List<VulkanCommandPool*> secondary_graphics_pools;

		//Transfer Command Pool
		VulkanCommandPool* transfer_pool = nullptr;

		//Descriptor Pools
		List<VulkanDescriptorPool*> descriptor_pools;

		//Pipeline Pool
		VulkanPipelinePool* pipeline_pool = nullptr;

		//RenderPass Pool
		VulkanRenderPassPool* render_pass_pool = nullptr;
	};
}