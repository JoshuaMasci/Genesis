#pragma once

#include "Genesis/Core/Types.hpp"
#include "Genesis/Rendering/RenderingTypes.hpp"
#include "Genesis/Rendering/PipelineSettings.hpp"

namespace Genesis
{
	enum class CommandBufferType
	{
		SingleThread,
		MultiThread
	};

	class CommandBuffer
	{
	public:
		virtual void setShader(Shader shader) = 0;
		virtual void setPipelineSettings(PipelineSettings& settings) = 0;
		
		virtual void setScissor(vector2I offset, vector2U extent) = 0;

		//Uniform
		virtual void setUniformFloat(string name, float value) = 0;
		virtual void setUniformVec2(string name, vector2F value) = 0;
		virtual void setUniformVec3(string name, vector3F value) = 0;
		virtual void setUniformVec4(string name, vector4F value) = 0;

		virtual void setUniformMat3(string name, matrix3F value) = 0;
		virtual void setUniformMat4(string name, matrix4F value) = 0;
		
		virtual void setUniformTexture(string name, Texture texture) = 0;
		virtual void setUniformView(string name, View view, uint16_t view_image_index) = 0;

		//Draw Calls
		virtual void drawIndexed(VertexBuffer vertex_handle, IndexBuffer index_handle) = 0;
		virtual void drawIndexedOffset(VertexBuffer vertex_handle, IndexBuffer index_handle, uint32_t index_offset, uint32_t index_count) = 0;

	protected:
	};
}