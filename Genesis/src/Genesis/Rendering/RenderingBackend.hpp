#pragma once

#include "Genesis/Core/VectorTypes.hpp"
#include "Genesis/Rendering/RenderingTypes.hpp"
#include "Genesis/Rendering/Camera.hpp"
#include "Genesis/Rendering/VertexInputDescription.hpp"
#include "Genesis/Rendering/FramebufferLayout.hpp"

namespace Genesis
{
	class RenderingBackend
	{
	public:
		virtual ~RenderingBackend() {};

		virtual bool beginFrame() = 0;
		virtual void endFrame() = 0;
		virtual void submitFrame(vector<ViewHandle> sub_views) = 0;

		virtual VertexBufferHandle createVertexBuffer(void* data, uint64_t data_size, VertexInputDescription& vertex_input_description, MemoryUsage memory_usage = MemoryUsage::GPU_Only) = 0;
		virtual void destroyVertexBuffer(VertexBufferHandle vertex_buffer_index) = 0;

		virtual IndexBufferHandle createIndexBuffer(void* indices, uint32_t indices_count, IndexType type, MemoryUsage memory_usage = MemoryUsage::GPU_Only) = 0;
		virtual void destroyIndexBuffer(IndexBufferHandle index_buffer_index) = 0;

		virtual UniformBufferHandle createUniformBuffer(string uniform_name, uint64_t size_bytes) = 0;
		virtual void fillUniformBuffer(UniformBufferHandle uniform_buffer_index, void* data, uint64_t uniform_size) = 0;
		virtual void destroyUniformBuffer(UniformBufferHandle uniform_buffer_index) = 0;

		virtual TextureHandle createTexture(vector2U size, void* data, uint64_t data_size) = 0;
		virtual void destroyTexture(TextureHandle texture_index) = 0;

		virtual ShaderHandle createShader(string vert_data, string frag_data) = 0;
		virtual void destroyShader(ShaderHandle shader_handle) = 0;

		virtual ViewHandle createView(vector2U size, FramebufferLayout& layout) = 0;
		virtual void destroyView(ViewHandle index) = 0;

		virtual void startView(ViewHandle index) = 0;
		virtual void endView(ViewHandle index) = 0;
		virtual void sumbitView(ViewHandle index) = 0;

		virtual void tempDrawScreen(VertexBufferHandle vertices_handle, IndexBufferHandle indices_handle, ShaderHandle shader_handle, TextureHandle texture_handle, UniformBufferHandle uniform_handle) = 0;
		
		//IMGUI TEMP
		virtual void tempDrawView(ViewHandle view_handle, VertexBufferHandle vertices_handle, IndexBufferHandle indices_handle, ShaderHandle shader_handle, TextureHandle texture_handle, uint32_t index_offset, uint32_t index_count, vector2I scissor_offest, vector2U scissor_extent) = 0;

		virtual matrix4F getPerspectiveMatrix(Camera* camera, float aspect_ratio) = 0;
		virtual matrix4F getPerspectiveMatrix(Camera* camera, ViewHandle view) = 0;
		//virtual matrix4F getorthographicMatrix();

		virtual vector2U getScreenSize() = 0;

		//Wait until all GPU proccessing is done
		virtual void waitTillDone() = 0;
	};
}