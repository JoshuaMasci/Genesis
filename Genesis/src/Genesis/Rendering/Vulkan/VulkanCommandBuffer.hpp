#pragma once

#include "Genesis/Rendering/CommandBuffer.hpp"
#include "Genesis/Rendering/PipelineSettings.hpp"
#include "Genesis/Rendering/Vulkan/VulkanInstance.hpp"
#include "Genesis/Rendering/Vulkan/VulkanPipline.hpp"

namespace Genesis
{
	class VulkanCommandBuffer : public CommandBuffer
	{
	public:
		VulkanCommandBuffer(VkCommandBuffer command_buffer, VkRenderPass render_pass);
		~VulkanCommandBuffer();

		virtual void setShader(ShaderHandle shader) override;
		virtual void setPipelineSettings(PipelineSettings& settings) override;

		virtual void setScissor(vector2U offset, vector2U extent) override;

		//Uniform
		virtual void setUniformFloat(string name, float value) override;
		virtual void setUniformVec4(string name, vector4F vector) override;
		virtual void setUniformMat4(string name, matrix4F matrix) override;
		virtual void setUniformTexture(string name, TextureHandle texture) override;
		virtual void setUniformView(string name, ViewHandle view, uint16_t view_image_index) override;

		//Draw Calls
		virtual void drawIndexed(VertexBufferHandle vertex_handle, IndexBufferHandle index_handle) override;
		virtual void drawIndexedOffset(VertexBufferHandle vertex_handle, IndexBufferHandle index_handle, uint32_t index_offset, uint32_t index_count) override;

	protected:
		VkCommandBuffer command_buffer;
		VkRenderPass render_pass;

		VulkanShader* current_shader = nullptr;
		PipelineSettings current_settings;
		VulkanPipline* current_pipeline = nullptr;
	};
}