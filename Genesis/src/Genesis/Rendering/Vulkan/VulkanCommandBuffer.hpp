#pragma once

#include "Genesis/Rendering/CommandBuffer.hpp"
#include "Genesis/Rendering/PipelineSettings.hpp"

#include "Genesis/Rendering/Vulkan/VulkanInclude.hpp"
#include "Genesis/Rendering/Vulkan/VulkanCommandPool.hpp"
#include "Genesis/Rendering/Vulkan/VulkanPipline.hpp"

#include "Genesis/Rendering/Vulkan/VulkanDescriptorPool.hpp"
#include "Genesis/Rendering/Vulkan/VulkanUniformPool.hpp"

namespace Genesis
{
	class VulkanCommandBuffer : public CommandBuffer
	{
	public:
		VulkanCommandBuffer(uint32_t thread_index, uint32_t frame_index, VulkanCommandPool* command_pool);
		~VulkanCommandBuffer();

		void beginCommandBuffer(VkRenderPassBeginInfo& render_pass_info, VkSubpassContents mode);
		void endCommandBuffer();
		void submitCommandBuffer(VkQueue queue, Array<VkSemaphore>& wait_semaphores, Array<VkPipelineStageFlags>& wait_states, Array<VkSemaphore>& signal_semaphores, VkFence trigger_fence);

		//Public functions begin
		virtual void setShader(ShaderHandle shader) override;
		virtual void setPipelineSettings(PipelineSettings& settings) override;

		virtual void setScissor(vector2U offset, vector2U extent) override;

		//Uniform
		virtual void setUniformFloat(string name, float value) override;
		virtual void setUniformVec4(string name, vector4F value) override;
		virtual void setUniformMat4(string name, matrix4F value) override;
		virtual void setUniformTexture(string name, TextureHandle texture) override;
		virtual void setUniformView(string name, ViewHandle view, uint16_t view_image_index) override;

		//Draw Calls
		virtual void drawIndexed(VertexBufferHandle vertex_handle, IndexBufferHandle index_handle) override;
		virtual void drawIndexedOffset(VertexBufferHandle vertex_handle, IndexBufferHandle index_handle, uint32_t index_offset, uint32_t index_count) override;

	protected:
		const uint32_t thread_index;
		const uint32_t frame_index;

		VkCommandBuffer command_buffer;
		VulkanCommandPool* command_pool = nullptr;

		//Utils
		VulkanDescriptorPool* descriptor_pool = nullptr;
		VulkanUniformPool* uniform_pool = nullptr;

		//Render State Info
		VkRenderPass render_pass;
		VulkanShader* current_shader = nullptr;
		PipelineSettings current_settings;
		VulkanPipline* current_pipeline = nullptr;

		//Uniform
		struct UniformData
		{
			enum
			{
				Float,
				Vector4,
				Matrix4,
				Image,
			} type;

			union
			{
				vector4F scalar;
				matrix4F matrix;
				VkImageView image;
			} data;
		};
		map<string, UniformData> uniforms;
	};
}