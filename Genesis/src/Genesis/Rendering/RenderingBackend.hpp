#pragma once

#include "Genesis/Core/VectorTypes.hpp"
#include "Genesis/Rendering/RenderingTypes.hpp"
#include "Genesis/Rendering/Camera.hpp"
#include "Genesis/Rendering/VertexInputDescription.hpp"
#include "Genesis/Rendering/FramebufferLayout.hpp"
#include "Genesis/Rendering/CommandBuffer.hpp"

namespace Genesis
{
	class RenderingBackend
	{
	public:
		virtual ~RenderingBackend() {};

		virtual void setScreenSize(vector2U size) = 0;
		virtual vector2U getScreenSize() = 0;

		virtual bool beginFrame() = 0;
		virtual void endFrame() = 0;
		virtual void submitFrame(vector<View> sub_views) = 0;

		virtual VertexBuffer createVertexBuffer(void* data, uint64_t data_size, VertexInputDescription& vertex_input_description, MemoryUsage memory_usage = MemoryUsage::GPU_Only) = 0;
		virtual void destroyVertexBuffer(VertexBuffer vertex_buffer_index) = 0;

		virtual IndexBuffer createIndexBuffer(void* indices, uint32_t indices_count, IndexType type, MemoryUsage memory_usage = MemoryUsage::GPU_Only) = 0;
		virtual void destroyIndexBuffer(IndexBuffer index_buffer_index) = 0;

		virtual Texture createTexture(vector2U size, void* data, uint64_t data_size) = 0;
		virtual void destroyTexture(Texture texture_index) = 0;

		virtual Shader createShader(string& vert_data, string& frag_data) = 0;
		virtual void destroyShader(Shader shader_handle) = 0;

		virtual View createView(vector2U size, FramebufferLayout& layout, CommandBufferType type) = 0;
		virtual void destroyView(View index) = 0;
		virtual void resizeView(View index, vector2U new_size) = 0;

		virtual void startView(View index) = 0;
		virtual void endView(View index) = 0;
		virtual void submitView(View index, vector<View> sub_views = {}) = 0;
		virtual CommandBuffer* getViewCommandBuffer(View index) = 0;

		//TEMP
		virtual CommandBuffer* getScreenCommandBuffer() = 0;

		virtual matrix4F getPerspectiveMatrix(Camera* camera, float aspect_ratio) = 0;
		virtual matrix4F getPerspectiveMatrix(Camera* camera, View view) = 0;
		//virtual matrix4F getorthographicMatrix();

		//Utils
		virtual VertexBuffer getWholeScreenQuadVertex() = 0;
		virtual IndexBuffer getWholeScreenQuadIndex() = 0;

		//Wait until all GPU proccessing is done
		virtual void waitTillDone() = 0;
	};
}