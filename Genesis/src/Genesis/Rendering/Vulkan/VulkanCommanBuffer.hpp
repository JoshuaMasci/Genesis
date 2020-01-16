#pragma once

#include "Genesis/Rendering/Vulkan/VulkanInclude.hpp"
#include "Genesis/Rendering/Vulkan/VulkanCommandPool.hpp"
#include "Genesis/Rendering/Vulkan/VulkanPipelinePool.hpp"
#include "Genesis/Rendering/Vulkan/VulkanDescriptorPool.hpp"
#include "Genesis/Rendering/Vulkan/VulkanSamplerPool.hpp"
#include "Genesis/Rendering/Vulkan/VulkanTransferBuffer.hpp"

#include "Genesis/Rendering/Vulkan/VulkanFramebuffer.hpp"

#include "Genesis/Rendering/CommandBuffer.hpp"

namespace Genesis
{
	class VulkanCommandBuffer
	{
	public:
		VulkanCommandBuffer(VulkanDevice* device, VulkanCommandPool* command_pool, VulkanThreadPipelinePool* pipeline_pool, VulkanDescriptorPool* descriptor_pool, uint32_t frame_index);
		~VulkanCommandBuffer();

		inline VkCommandBuffer getCommandBuffer() { return this->command_buffer; };

		void startPrimary(VkFramebuffer framebuffer, VkRenderPass render_pass, VkRect2D render_area, List<VkClearValue>& clear_values, VkSubpassContents content);
		void startSecondary(VkFramebuffer framebuffer, VkRenderPass render_pass);
		void end();
		//void submit(VkQueue queue, List<VkSemaphore>& wait_semaphores, List<VkPipelineStageFlags>& wait_states, List<VkSemaphore>& signal_semaphores, VkFence trigger_fence);

		void executeSecondary(List<VkCommandBuffer>& secondary_command_buffers);

		void setEvent(VkEvent trigger_event, VkPipelineStageFlags trigger_stage);
		void waitEvent(VkEvent trigger_event);

		void setShader(VulkanShader* shader);
		void setPipelineSettings(PipelineSettings& settings);
		void setScissor(VkRect2D& scissor_rect);

		void setUniformBuffer(uint32_t set, uint32_t binding, VkBuffer buffer, uint64_t buffer_size);
		void setUniformTexture(uint32_t set, uint32_t binding, VkImageView texture, VkSampler sampler);
		void setUniformConstant(void* data, uint32_t data_size);

		void setVertexBuffer(VkBuffer vertex_buffer, VertexInputDescription* vertex_description);
		void setIndexBuffer(VkBuffer index_buffer, IndexType type);

		void drawIndexed(uint32_t index_count, uint32_t index_offset = 0, uint32_t instance_count = 1, uint32_t instance_offset = 0);

		inline const uint32_t getFrameIndex() { return this->FRAME_INDEX; };

	protected:
		const uint32_t FRAME_INDEX;

		void bindDescriptors();

		VulkanDevice* device = VK_NULL_HANDLE;
		VulkanCommandPool* command_pool = nullptr;
		VulkanThreadPipelinePool* pipeline_pool = nullptr;
		VulkanDescriptorPool* descriptor_pool = nullptr;

		VkCommandBuffer command_buffer = VK_NULL_HANDLE;

		struct DescriptorBinding
		{
			VkDescriptorType type = VK_DESCRIPTOR_TYPE_MAX_ENUM;
			union
			{
				struct
				{
					VkBuffer buffer;
					uint64_t size;
				} uniform_buffer;
				struct
				{
					VkImageView image;
					VkSampler sampler;
				} image_sampler;
			} data;
		};

		struct DescriptorSet
		{
			VkDescriptorSetLayout layout = VK_NULL_HANDLE;
			List<DescriptorBinding> bindings;

			bool has_changed = true;
			VkDescriptorSet last_set = VK_NULL_HANDLE;
		};

		//Framebuffer Info
		VkFramebuffer current_framebuffer = VK_NULL_HANDLE;
		VkRenderPass current_render_pass = VK_NULL_HANDLE;
		//VkRect2D current_render_area;

		//Shader Info
		VulkanShader* current_shader = nullptr;
		List<DescriptorSet> current_descriptor_sets;

		//Mesh info
		VertexInputDescription* current_vertex_description = nullptr;

		//Pipeline State
		PipelineSettings current_pipeline_settings;
		VkPipelineLayout current_pipeline_layout = VK_NULL_HANDLE;
		VkPipeline current_pipeline = VK_NULL_HANDLE;
	};

	class VulkanCommandBufferSingle : public CommandBuffer
	{
	public:
		VulkanCommandBufferSingle(VulkanDevice* device, VulkanCommandPool* command_pool, VulkanThreadPipelinePool* pipeline_pool, VulkanDescriptorPool* descriptor_pool, VulkanSamplerPool* sampler_pool, VulkanTransferBuffer* transfer_buffer, uint32_t frame_index);
		~VulkanCommandBufferSingle();

		inline VkCommandBuffer getCommandBuffer() { return this->command_buffer.getCommandBuffer(); };

		void start(VkFramebuffer framebuffer, VkRenderPass render_pass, VkRect2D render_area, List<VkClearValue>& clear_values, VkSubpassContents content);
		void end();
		//void submit(VkQueue queue, List<VkSemaphore>& wait_semaphores, List<VkPipelineStageFlags>& wait_states, List<VkSemaphore>& signal_semaphores, VkFence trigger_fence);

		void setEvent(VkEvent trigger_event, VkPipelineStageFlags trigger_stage);

		virtual void setShader(Shader shader) override;
		virtual void setPipelineSettings(PipelineSettings & settings) override;
		virtual void setScissor(vector2I offset, vector2U extent) override;
		virtual void setUniformBuffer(uint32_t set, uint32_t binding, UniformBuffer buffer) override;
		virtual void setUniformTexture(uint32_t set, uint32_t binding, Texture texture, Sampler& sampler) override;
		virtual void setUniformView(uint32_t set, uint32_t binding, View view, uint8_t view_image_index, Sampler& sampler) override;
		virtual void setUniformConstant(void * data, uint32_t data_size) override;
		virtual void setVertexBuffer(VertexBuffer vertex, VertexInputDescription& vertex_description) override;
		virtual void setIndexBuffer(IndexBuffer index, IndexType type) override;
		virtual void drawIndexed(uint32_t index_count, uint32_t index_offset = 0, uint32_t instance_count = 1, uint32_t instance_offset = 0) override;
	private:
		VulkanCommandBuffer command_buffer;

		VulkanSamplerPool* sampler_pool = nullptr;
		VulkanTransferBuffer* transfer_buffer = nullptr;
	};
}