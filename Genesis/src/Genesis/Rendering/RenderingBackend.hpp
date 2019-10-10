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
		virtual void submitFrame(vector<ViewHandle> sub_views) = 0;

		virtual VertexBufferHandle createVertexBuffer(void* data, uint64_t data_size, VertexInputDescription& vertex_input_description, MemoryUsage memory_usage = MemoryUsage::GPU_Only) = 0;
		virtual void destroyVertexBuffer(VertexBufferHandle vertex_buffer_index) = 0;

		virtual IndexBufferHandle createIndexBuffer(void* indices, uint32_t indices_count, IndexType type, MemoryUsage memory_usage = MemoryUsage::GPU_Only) = 0;
		virtual void destroyIndexBuffer(IndexBufferHandle index_buffer_index) = 0;

		virtual TextureHandle createTexture(vector2U size, void* data, uint64_t data_size) = 0;
		virtual void destroyTexture(TextureHandle texture_index) = 0;

		virtual ShaderHandle createShader(string vert_data, string frag_data) = 0;
		virtual void destroyShader(ShaderHandle shader_handle) = 0;

		virtual ViewHandle createView(vector2U size, FramebufferLayout& layout, CommandBufferType type) = 0;
		virtual void destroyView(ViewHandle index) = 0;
		virtual void resizeView(ViewHandle index, vector2U new_size) = 0;

		virtual void startView(ViewHandle index) = 0;
		virtual void endView(ViewHandle index) = 0;
		virtual void sumbitView(ViewHandle index) = 0;
		virtual CommandBuffer* getViewCommandBuffer(ViewHandle index) = 0;

		//TEMP
		virtual CommandBuffer* getScreenCommandBuffer() = 0;

		virtual matrix4F getPerspectiveMatrix(Camera* camera, float aspect_ratio) = 0;
		virtual matrix4F getPerspectiveMatrix(Camera* camera, ViewHandle view) = 0;
		//virtual matrix4F getorthographicMatrix();

		//Utils
		virtual VertexBufferHandle getWholeScreenQuadVertex() = 0;
		virtual IndexBufferHandle getWholeScreenQuadIndex() = 0;

		//Wait until all GPU proccessing is done
		virtual void waitTillDone() = 0;
	};
}