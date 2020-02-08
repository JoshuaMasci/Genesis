#pragma once

#include "VulkanInclude.hpp"
#include "VulkanCommandPool.hpp"
#include "VulkanPipelinePool.hpp"
#include "VulkanDescriptorPool.hpp"
#include "VulkanSamplerPool.hpp"
#include "VulkanTransferBuffer.hpp"

#include "VulkanFramebuffer.hpp"

#include "Genesis/Rendering/CommandBuffer.hpp"

namespace Genesis
{
	class VulkanCommandBufferInternal
	{
	public:
		VulkanCommandBufferInternal(VulkanDevice* device, VulkanCommandPool* command_pool, VulkanThreadPipelinePool* pipeline_pool, VulkanDescriptorPool* descriptor_pool, uint32_t frame_index);
		~VulkanCommandBufferInternal();

		inline VkCommandBuffer getCommandBuffer() { return this->command_buffer; };

		void startPrimary(VkFramebuffer framebuffer, VkRenderPass render_pass, VkRect2D render_area, List<VkClearValue>& clear_values, VkSubpassContents content);
		void startSecondary(VkFramebuffer framebuffer, VkRenderPass render_pass, VkRect2D render_area);
		void endPrimary();
		void endSecondary();

		void setShader(VulkanShader* shader);
		void setPipelineSettings(PipelineSettings& settings);
		void setScissor(VkRect2D& scissor_rect);

		void setUniformBuffer(uint32_t set, uint32_t binding, VkBuffer buffer, uint64_t buffer_size);
		void setUniformTexture(uint32_t set, uint32_t binding, VkImageView texture, VkSampler sampler);
		void setUniformConstant(void* data, uint32_t data_size);

		void setVertexBuffer(VkBuffer vertex_buffer, VulkanVertexInputDescription* vertex_description);
		void setIndexBuffer(VkBuffer index_buffer, VkIndexType type);

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
		VulkanVertexInputDescription* current_vertex_description = nullptr;

		//Pipeline State
		PipelineSettings current_pipeline_settings;
		VkPipelineLayout current_pipeline_layout = VK_NULL_HANDLE;
		VkPipeline current_pipeline = VK_NULL_HANDLE;
	};

	class VulkanCommandBuffer : public CommandBuffer
	{
	public:
		VulkanCommandBuffer(VulkanDevice* device, VulkanCommandPool* command_pool, VulkanThreadPipelinePool* pipeline_pool, VulkanDescriptorPool* descriptor_pool, VulkanTransferBuffer* transfer_buffer, uint32_t frame_index);
		~VulkanCommandBuffer();

		inline VkCommandBuffer getCommandBuffer() { return this->command_buffer.getCommandBuffer(); };

		void startPrimary(VkFramebuffer framebuffer, VkRenderPass render_pass, VkRect2D render_area, List<VkClearValue>& clear_values, VkSubpassContents content);
		void startSecondary(VkFramebuffer framebuffer, VkRenderPass render_pass, VkRect2D render_area);
		void endPrimary();
		void endSecondary();

		virtual void setShader(Shader shader) override;
		virtual void setPipelineSettings(PipelineSettings & settings) override;
		virtual void setScissor(vector2I offset, vector2U extent) override;
		virtual void setUniformBuffer(uint32_t set, uint32_t binding, DynamicBuffer uniform_buffer) override;
		virtual void setUniformTexture(uint32_t set, uint32_t binding, Texture texture, Sampler sampler) override;
		virtual void setUniformFramebuffer(uint32_t set, uint32_t binding, Framebuffer framebuffer, uint8_t framebuffer_image_index, Sampler sampler) override;
		virtual void setUniformConstant(void * data, uint32_t data_size) override;
		virtual void setVertexBuffer(StaticBuffer vertex_buffer, VertexInputDescription& vertex_description) override;
		virtual void setIndexBuffer(StaticBuffer index_buffer, IndexType type) override;
		virtual void drawIndexed(uint32_t index_count, uint32_t index_offset = 0, uint32_t instance_count = 1, uint32_t instance_offset = 0) override;
	private:
		VulkanCommandBufferInternal command_buffer;

		VulkanTransferBuffer* transfer_buffer = nullptr;
	};

	class VulkanCommandBufferMultithread
	{
	public:
		VulkanCommandBufferMultithread(VulkanDevice* device, uint32_t thread_count, VulkanCommandPool* primary_pool, List<VulkanCommandPool*> secondary_pools, List<VulkanThreadPipelinePool*> pipeline_pools, List<VulkanDescriptorPool*> descriptor_pools, VulkanTransferBuffer* transfer_buffer, uint32_t frame_index);
		~VulkanCommandBufferMultithread();

		void start(VulkanFramebuffer* framebuffer_target);
		void end();

		inline VkCommandBuffer getCommandBuffer() { return this->primary_buffer; };
		inline List<VulkanCommandBuffer*>* getSecondaryCommandBuffers()
		{
			return &this->secondary_buffers;
		};

	private:
		VulkanCommandPool* primary_pool;
		VkCommandBuffer primary_buffer;

		List<VulkanCommandBuffer*> secondary_buffers;
	};

	struct VulkanCommandBufferMultithreadSet
	{
		VulkanCommandBufferMultithreadSet(VulkanDevice* device, uint32_t frame_count, uint32_t thread_count, VulkanCommandPool* primary_pool, List<VulkanCommandPool*> secondary_pools, List<VulkanThreadPipelinePool*> pipeline_pools, List<VulkanDescriptorPool*> descriptor_pools, List<VulkanTransferBuffer*> transfer_buffers);
		~VulkanCommandBufferMultithreadSet();

		List<VulkanCommandBufferMultithread*> command_buffers;
	};

	struct VulkanCommandBufferSet
	{
		VulkanCommandBufferSet(VulkanDevice* device, VulkanCommandPool* command_pool, VulkanThreadPipelinePool* pipeline_pool, VulkanDescriptorPool* descriptor_pool, VulkanTransferBuffer* transfer_buffer, uint32_t frame_count);
		~VulkanCommandBufferSet();

		List<VulkanCommandBuffer*> command_buffers;
	};
}