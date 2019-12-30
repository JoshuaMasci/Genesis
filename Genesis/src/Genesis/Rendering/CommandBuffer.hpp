#pragma once

#include "Genesis/Core/Types.hpp"
#include "Genesis/Rendering/RenderingTypes.hpp"
#include "Genesis/Rendering/PipelineSettings.hpp"
#include "Genesis/Rendering/VertexInputDescription.hpp"

namespace Genesis
{
	class CommandBuffer
	{
	public:
		virtual void setShader(Shader shader) = 0;
		virtual void setPipelineSettings(PipelineSettings& settings) = 0;
		virtual void setScissor(vector2I offset, vector2U extent) = 0;

		virtual void setUniformBuffer(uint32_t set, uint32_t binding, UniformBuffer buffer) = 0;
		virtual void setUniformTexture(uint32_t set, uint32_t binding, Texture texture) = 0;
		virtual void setUniformView(uint32_t set, uint32_t binding, View view, uint8_t view_image_index) = 0;

		virtual void setUniformConstant(void* data, uint32_t data_size) = 0;

		virtual void setVertexBuffer(VertexBuffer vertex, VertexInputDescription& vertex_description) = 0;
		virtual void setIndexBuffer(IndexBuffer index, IndexType type) = 0;

		//Draw Calls
		virtual void drawIndexed(uint32_t index_count, uint32_t index_offset = 0, uint32_t instance_count = 1, uint32_t instance_offset = 0) = 0;
	};
}