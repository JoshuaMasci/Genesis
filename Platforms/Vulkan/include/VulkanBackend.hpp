#pragma once

#include "Genesis/RenderingBackend/RenderingBackend.hpp"
#include "Genesis/RenderingBackend/VertexInputDescription.hpp"
#include "Genesis/Platform/Window.hpp"

#include "VulkanInclude.hpp"
#include "VulkanInstance.hpp"
#include "VulkanDebugLayer.hpp"
#include "VulkanDevice.hpp"
#include "VulkanSwapchain.hpp"

#include "VulkanCommandPool.hpp"
#include "VulkanDescriptorPool.hpp"
#include "VulkanPipelinePool.hpp"
#include "VulkanRenderPassPool.hpp"
#include "VulkanTransferBuffer.hpp"
#include "VulkanLayoutPool.hpp"
#include "VulkanSamplerPool.hpp"
#include "VulkanVertexInputPool.hpp"

#include "VulkanBuffer.hpp"
#include "VulkanTexture.hpp"
#include "VulkanCommanBuffer.hpp"

#include "Genesis/RenderingBackend/DelayedResourceDeleter.hpp"

namespace Genesis
{
	class VulkanBackend : public RenderingBackend
	{
	public:
		VulkanBackend(Window* window, uint32_t number_of_threads);
		~VulkanBackend();

		virtual void setScreenSize(vector2U size) override;
		virtual vector2U getScreenSize() override;

		virtual CommandBufferInterface* beginFrame() override;
		virtual void endFrame() override;

		virtual Sampler createSampler(const SamplerCreateInfo& create_info) override;

		virtual VertexInputDescription createVertexInputDescription(vector<VertexElementType> input_elements) override;
		virtual VertexInputDescription createVertexInputDescription(const VertexInputDescriptionCreateInfo& create_info) override;

		virtual DescriptorSetLayout createDescriptorSetLayout(const DescriptorSetLayoutCreateInfo& create_info) override;
		virtual DescriptorSet createDescriptorSet(const DescriptorSetCreateInfo& create_info) override;
		virtual void destroyDescriptorSet(DescriptorSet descriptor_set) override;

		virtual PipelineLayout createPipelineLayout(const PipelineLayoutCreateInfo& create_info) override;

		virtual ShaderModule createShaderModule(ShaderModuleCreateInfo& create_info) override;
		virtual void destroyShaderModule(ShaderModule shader_module) override;

		virtual StaticBuffer createStaticBuffer(void* data, uint64_t data_size, BufferUsage buffer_usage, MemoryType memory_usage = MemoryType::GPU_Only) override;
		virtual void destroyStaticBuffer(StaticBuffer buffer) override;

		virtual DynamicBuffer createDynamicBuffer(uint64_t data_size, BufferUsage buffer_usage, MemoryType memory_usage = MemoryType::CPU_Visable) override;
		virtual void destroyDynamicBuffer(DynamicBuffer buffer) override;
		virtual void writeDynamicBuffer(DynamicBuffer buffer, void* data, uint64_t data_size) override;

		virtual Texture createTexture(vector2U size, void* data, uint64_t data_size) override;
		virtual void destroyTexture(Texture texture) override;

		virtual Shader createShader(string& vert_data, string& frag_data) override;
		virtual void destroyShader(Shader shader) override;

		virtual Framebuffer createFramebuffer(FramebufferLayout& layout, vector2U size) override;
		virtual void destroyFramebuffer(Framebuffer framebuffer) override;
		virtual void resizeFramebuffer(Framebuffer framebuffer, vector2U size) override;

		virtual STCommandBuffer createSTCommandBuffer() override;
		virtual void destroySTCommandBuffer(STCommandBuffer st_command_buffer) override;
		virtual CommandBufferInterface* beginSTCommandBuffer(STCommandBuffer st_command_buffer, Framebuffer framebuffer_target) override;
		virtual void endSTCommandBuffer(STCommandBuffer st_command_buffer) override;

		virtual MTCommandBuffer createMTCommandBuffer() override;
		virtual void destroyMTCommandBuffer(MTCommandBuffer mt_command_buffer) override;
		virtual List<CommandBufferInterface*>* beginMTCommandBuffer(MTCommandBuffer mt_command_buffer, Framebuffer framebuffer_target) override;
		virtual void endMTCommandBuffer(MTCommandBuffer mt_command_buffer) override;

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

		vector<VkCommandBuffer> graphics_command_buffers;

		struct Frame
		{
			VkSemaphore image_ready_semaphore = VK_NULL_HANDLE;

			VulkanCommandBuffer* command_buffer = nullptr;
			VkSemaphore command_buffer_done_semaphore = VK_NULL_HANDLE;
			VkFence frame_done_fence = VK_NULL_HANDLE;
		};
		List<Frame> frames;

		//Graphics Command Pools
		VulkanCommandPool* primary_graphics_pool = nullptr;
		List<VulkanCommandPool*> secondary_graphics_pools;

		//Transfer Command Pool
		VulkanCommandPool* transfer_pool = nullptr;
		List <VulkanTransferBuffer*> transfer_buffers;

		//Descriptor Pools
		VulkanDescriptorPool2* descriptor_pool = nullptr;
		List<VulkanDescriptorPool*> descriptor_pools;
		VulkanLayoutPool* layout_pool = nullptr;

		//Pipeline Pool
		VulkanPipelinePool* pipeline_pool = nullptr;
		List<VulkanThreadPipelinePool*> thread_pipeline_pools;

		//RenderPass Pool
		VulkanRenderPassPool* render_pass_pool = nullptr;

		VulkanSamplerPool* sampler_pool = nullptr;
		VulkanVertexInputPool* vertex_input_pool = nullptr;

		//Resource
		DelayedResourceDeleter<VulkanBuffer>* buffer_deleter = nullptr;
		DelayedResourceDeleter<VulkanDynamicBuffer>* dynamic_deleter = nullptr;
		DelayedResourceDeleter<VulkanTexture>* texture_deleter = nullptr;
		DelayedResourceDeleter<VulkanShader>* shader_deleter = nullptr;
		DelayedResourceDeleter<VulkanFramebufferSet>* frame_deleter = nullptr;
		DelayedResourceDeleter<VulkanCommandBufferSet>* st_command_buffer_deleter = nullptr;
		DelayedResourceDeleter<VulkanCommandBufferMultithreadSet>* mt_command_buffer_deleter = nullptr;
	};
}