#pragma once

#include "Genesis/Core/Types.hpp"
#include "Genesis/Rendering/RenderingTypes.hpp"
#include "Genesis/Rendering/PipelineSettings.hpp"

namespace Genesis
{
	class CommandBuffer
	{
	public:
		virtual void setShader(Shader shader) = 0;
		virtual void setPipelineSettings(PipelineSettings& settings) = 0;
		
		virtual void setScissor(vector2I offset, vector2U extent) = 0;


		//Draw Calls
		virtual void drawIndexed(VertexBuffer vertex_handle, IndexBuffer index_handle) = 0;
		virtual void drawIndexedOffset(VertexBuffer vertex_handle, IndexBuffer index_handle, uint32_t index_offset, uint32_t index_count) = 0;

	protected:
	};
}