#pragma once

#include "Genesis/Rendering/CommandBuffer.hpp"
#include "Genesis/Rendering/PipelineSettings.hpp"

#include "Genesis/Rendering/Vulkan/VulkanInclude.hpp"
#include "Genesis/Rendering/Vulkan/VulkanCommandPool.hpp"
#include "Genesis/Rendering/Vulkan/VulkanPipline.hpp"

#include "Genesis/Rendering/Vulkan/VulkanPipelinePool.hpp"
#include "Genesis/Rendering/Vulkan/VulkanDescriptorPool.hpp"
#include "Genesis/Rendering/Vulkan/VulkanUniformPool.hpp"

namespace Genesis
{
	class VulkanCommandBuffer : public CommandBuffer
	{
	public:
		VulkanCommandBuffer(uint32_t thread_index, uint32_t frame_index, VkDevice device, VulkanCommandPool* command_pool, VulkanPipelinePool* pipeline_pool, VulkanDescriptorPool* descriptor_pool, VulkanUniformPool* uniform_pool, VkSampler temp_sampler);
		~VulkanCommandBuffer();

		void beginCommandBufferPrimary(VkRenderPassBeginInfo& render_pass_info);
		void beginCommandBufferSecondary(VkCommandBufferInheritanceInfo& inheritance_info, VkExtent2D size);
		void endCommandBuffer();
		void submitCommandBuffer(VkQueue queue, Array<VkSemaphore>& wait_semaphores, Array<VkPipelineStageFlags>& wait_states, Array<VkSemaphore>& signal_semaphores, VkFence trigger_fence);
		void setUniform(const string& name, void* data, uint32_t data_size);

		inline VkCommandBuffer get() { return this->command_buffer; };

		//Public functions begin
		virtual void setShader(Shader shader) override;
		virtual void setPipelineSettings(PipelineSettings& settings) override;

		virtual void setScissor(vector2I offset, vector2U extent) override;

		//Uniform
		virtual void setUniformFloat(const string& name, float value) override;
		virtual void setUniformVec2(const string& name, vector2F value) override;
		virtual void setUniformVec3(const string& name, vector3F value) override;
		virtual void setUniformVec4(const string& name, vector4F value) override;

		virtual void setUniformMat3(const string& name, matrix3F value) override;
		virtual void setUniformMat4(const string& name, matrix4F value) override;
		virtual void setUniformTexture(const string& name, Texture texture) override;
		virtual void setUniformView(const string& name, View view, uint16_t view_image_index) override;

		//Draw Calls
		virtual void drawIndexed(VertexBuffer vertex_handle, IndexBuffer index_handle) override;
		virtual void drawIndexedOffset(VertexBuffer vertex_handle, IndexBuffer index_handle, uint32_t index_offset, uint32_t index_count) override;

	protected:
		const uint32_t thread_index;
		const uint32_t frame_index;

		VkDevice device;
		VkCommandBuffer command_buffer;
		VulkanCommandPool* command_pool = nullptr;

		//Utils
		VulkanPipelinePool* pipeline_pool;
		VulkanDescriptorPool* descriptor_pool = nullptr;
		VulkanUniformPool* uniform_pool = nullptr;
		VkSampler sampler; //TODO sampler system;


		//Render State Info
		VkRenderPass render_pass;
		VkExtent2D size;
		VulkanShader* current_shader = nullptr;
		PipelineSettings current_settings;
		VulkanPipline* current_pipeline = nullptr;

		bool has_descriptor_set_changed = true;
		VkDescriptorSet last_descriptor_set = VK_NULL_HANDLE;

		//Bindings
		map<uint32_t, Array<uint8_t>> binding_uniform_buffers;
		map<uint32_t, VkImageView> binding_image;
		map<VkShaderStageFlagBits, Array<uint8_t>> push_constant_blocks;
	};
}