#pragma once

#include "Genesis/Core/VectorTypes.hpp"
#include "Genesis/Rendering/RenderingTypes.hpp"
#include "Genesis/Rendering/Camera.hpp"
#include "Genesis/Rendering/VertexInputDescription.hpp"

namespace Genesis
{
	class RenderingBackend
	{
	public:
		virtual ~RenderingBackend() {};

		virtual bool beginFrame() = 0;
		virtual void endFrame() = 0;
		virtual void submitFrame(vector<ViewHandle> sub_views) = 0;

		virtual ShaderHandle createShader(string vert_data, string frag_data) = 0;
		virtual void destroyShader(ShaderHandle shader_handle) = 0;

		//virtual BufferIndex createBuffer(BufferType type, MemoryUsage memory_usage, void* data, uint64_t data_size) = 0;
		virtual BufferHandle createBuffer(uint64_t size_bytes, BufferType type, MemoryUsage memory_usage) = 0;
		virtual void fillBuffer(BufferHandle buffer_index, void* data, uint64_t data_size) = 0;
		virtual void destroyBuffer(BufferHandle buffer_index) = 0;

		//virtual TextureIndex createTexture(vector2U size, void* data, uint64_t data_size) = 0;
		virtual TextureHandle createTexture(vector2U size) = 0;
		virtual void fillTexture(TextureHandle texture_index, void* data, uint64_t data_size) = 0;
		virtual void destroyTexture(TextureHandle texture_index) = 0;

		virtual ViewHandle createView(ViewType type, vector2U size) = 0;
		virtual void destroyView(ViewHandle index) = 0;

		virtual void startView(ViewHandle index) = 0;
		virtual void endView(ViewHandle index) = 0;
		virtual void sumbitView(ViewHandle index) = 0;
		virtual void drawMeshView(ViewHandle index, uint32_t thread, BufferHandle vertices_index, BufferHandle indices_index, uint32_t indices_count, TextureHandle texture_index, matrix4F mvp) = 0;

		//TEMP
		virtual void drawMeshScreen(uint32_t thread, BufferHandle vertices_index, BufferHandle indices_index, uint32_t indices_count, TextureHandle texture_index, matrix4F mvp) = 0;
		virtual void drawMeshScreenViewTextured(uint32_t thread, BufferHandle vertices_index, BufferHandle indices_index, uint32_t indices_count, ViewHandle view_index, matrix4F mvp) = 0;

		virtual matrix4F getPerspectiveMatrix(Camera* camera, float aspect_ratio) = 0;
		virtual matrix4F getPerspectiveMatrix(Camera* camera, ViewHandle view) = 0;
		//virtual matrix4F getorthographicMatrix();

		virtual vector2U getScreenSize() = 0;

		//Wait until all GPU proccessing is done
		virtual void waitTillDone() = 0;

	private:
		uint32_t current_frame = 0;
	};
}