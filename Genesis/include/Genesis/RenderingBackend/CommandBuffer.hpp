#pragma once

#include "Genesis/Core/Types.hpp"
#include "Genesis/RenderingBackend/RenderingTypes.hpp"
#include "Genesis/RenderingBackend/PipelineSettings.hpp"
#include "Genesis/RenderingBackend/VertexInputDescription.hpp"

namespace Genesis
{
	class CommandBufferInterface
	{
	public:
		virtual void setShader(Shader shader) = 0;
		virtual void setPipelineSettings(PipelineSettings& settings) = 0;
		virtual void setScissor(vector2I offset, vector2U extent) = 0;

		virtual void setUniformBuffer(uint32_t set, uint32_t binding, DynamicBuffer buffer) = 0;
		virtual void setUniformTexture(uint32_t set, uint32_t binding, Texture texture, Sampler sampler) = 0;
		virtual void setUniformFramebuffer(uint32_t set, uint32_t binding, Framebuffer framebuffer, uint8_t framebuffer_image_index, Sampler sampler) = 0;

		virtual void setUniformConstant(void* data, uint32_t data_size) = 0;

		virtual void setVertexBuffer(StaticBuffer vertex_buffer, VertexInputDescription& vertex_description) = 0;
		virtual void setIndexBuffer(StaticBuffer index_buffer, IndexType type) = 0;

		//Draw Calls
		virtual void drawIndexed(uint32_t index_count, uint32_t index_offset = 0, uint32_t instance_count = 1, uint32_t instance_offset = 0) = 0;
	};
}