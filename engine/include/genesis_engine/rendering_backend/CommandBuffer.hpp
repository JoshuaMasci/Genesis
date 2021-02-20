#pragma once

#include "genesis_engine/renderingBackend/RenderingTypes.hpp"
#include "genesis_engine/renderingBackend/Sampler.hpp"
#include "genesis_engine/renderingBackend/DescriptorSet.hpp"
#include "genesis_engine/renderingBackend/PipelineSettings.hpp"
#include "genesis_engine/renderingBackend/VertexInputDescription.hpp"

namespace genesis_engine
{
	class CommandBufferInterface
	{
	public:
		virtual void setShader(Shader shader) = 0;
		virtual void setPipelineSettings(PipelineSettings& settings) = 0;
		virtual void setScissor(vec2i offset, vec2u extent) = 0;

		virtual void setUniformDynamicBuffer(uint32_t set, uint32_t binding, DynamicBuffer buffer) = 0;
		virtual void setUniformStaticBuffer(uint32_t set, uint32_t binding, StaticBuffer buffer) = 0;
		virtual void setUniformTexture(uint32_t set, uint32_t binding, Texture2D texture, Sampler sampler) = 0;
		virtual void setUniformFramebuffer(uint32_t set, uint32_t binding, Framebuffer framebuffer, uint8_t framebuffer_image_index, Sampler sampler) = 0;

		virtual void setDescriptorSet(uint32_t index, DescriptorSet descriptor_set) = 0;
		virtual void setUniformConstant(void* data, uint32_t data_size) = 0;

		virtual void setVertexBuffer(StaticBuffer vertex_buffer, VertexInputDescription& vertex_description) = 0;
		virtual void setIndexBuffer(StaticBuffer index_buffer, IndexType type) = 0;

		//Draw Calls
		virtual void drawIndexed(uint32_t index_count, uint32_t index_offset = 0, uint32_t instance_count = 1, uint32_t instance_offset = 0) = 0;
	};
}