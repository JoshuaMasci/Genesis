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
#include "Genesis/Rendering/Vulkan/VulkanLayoutPool.hpp"

#include "Genesis/Rendering/Vulkan/VulkanBuffer.hpp"
#include "Genesis/Rendering/Vulkan/VulkanTexture.hpp"
//#include "Genesis/Rendering/Vulkan/VulkanFramebuffer.hpp"
//#include "Genesis/Rendering/Vulkan/VulkanCommanBuffer.hpp"
#include "Genesis/Rendering/Vulkan/VulkanView.hpp"

#include "Genesis/Rendering/DelayedResourceDeleter.hpp"

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
		virtual void destroyVertexBuffer(VertexBuffer vertex_buffer) override;

		virtual IndexBuffer createIndexBuffer(void* data, uint64_t data_size, IndexType type, MemoryUsage memory_usage = MemoryUsage::GPU_Only) override;
		virtual void destroyIndexBuffer(IndexBuffer index_buffer) override;

		virtual UniformBuffer createUniformBuffer(uint64_t data_size, MemoryUsage memory_usage = MemoryUsage::CPU_Visable) override;
		virtual void destroyUniformBuffer(UniformBuffer uniform_buffer) override;
		virtual void setUniform(UniformBuffer uniform_buffer, void* data, uint64_t data_size) override;

		virtual Texture createTexture(vector2U size, void* data, uint64_t data_size) override;
		virtual void destroyTexture(Texture texture) override;

		virtual Shader createShader(string& vert_data, string& frag_data) override;
		virtual void destroyShader(Shader shader) override;

		/*virtual Framebuffer createFramebuffer(FramebufferLayout& layout, vector2U size) override;
		virtual void destroyFramebuffer(Framebuffer frame_buffer) override;
		virtual void resizeFramebuffer(Framebuffer frame_buffer, vector2U size) override;

		virtual CommandBuffer* createCommandBuffer() override;
		virtual void destroyCommandBuffer(CommandBuffer* command_buffer) override;
		virtual void beginCommandBuffer(CommandBuffer* command_buffer, Framebuffer target) override;
		virtual void endCommandBuffer(CommandBuffer* command_buffer) override;
		virtual void submitCommandBuffer(CommandBuffer* command_buffer) override;*/

		virtual View createView(FramebufferLayout& layout, vector2U size) override;
		virtual void destroyView(View view) override;
		virtual void resizeView(View view, vector2U size) override;
		virtual CommandBuffer* beginView(View view) override;
		virtual void endView(View view) override;

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
		VulkanLayoutPool* layout_pool = nullptr;

		//Pipeline Pool
		VulkanPipelinePool* pipeline_pool = nullptr;
		List<VulkanThreadPipelinePool*> thread_pipeline_pools;

		//RenderPass Pool
		VulkanRenderPassPool* render_pass_pool = nullptr;

		//Resource
		DelayedResourceDeleter<VulkanBuffer>* buffer_deleter = nullptr;
		DelayedResourceDeleter<VulkanUniformBuffer>* uniform_deleter = nullptr;
		DelayedResourceDeleter<VulkanTexture>* texture_deleter = nullptr;
		DelayedResourceDeleter<VulkanShader>* shader_deleter = nullptr;
		//DelayedResourceDeleter<VulkanFramebufferSet>* framebuffer_set_deleter = nullptr;
		//DelayedResourceDeleter<VulkanCommandBufferSingle>* single_command_buffer_deleter = nullptr;
		DelayedResourceDeleter<VulkanViewSingleThread>* view_deleter = nullptr;
	};
}