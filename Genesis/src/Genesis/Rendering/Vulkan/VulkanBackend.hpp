#pragma once

#include "Genesis/Rendering/RenderingBackend.hpp"
#include "Genesis/Platform/Window.hpp"

namespace Genesis
{
	//Prototype
	class VulkanInstance;

	class VulkanBackend : public RenderingBackend
	{
	public:
		VulkanBackend(Window* window, uint32_t number_of_threads);
		~VulkanBackend();

		virtual void setScreenSize(vector2U size) override;
		virtual vector2U getScreenSize() override;

		virtual bool beginFrame() override;
		virtual void endFrame() override;
		virtual void submitFrame(vector<View> sub_views) override;

		virtual VertexBuffer createVertexBuffer(void* data, uint64_t data_size, VertexInputDescription& vertex_input_description, MemoryUsage memory_usage = MemoryUsage::GPU_Only) override;
		virtual void destroyVertexBuffer(VertexBuffer vertex_buffer_index) override;

		virtual IndexBuffer createIndexBuffer(void* indices, uint32_t indices_count, IndexType type, MemoryUsage memory_usage = MemoryUsage::GPU_Only) override;
		virtual void destroyIndexBuffer(IndexBuffer index_buffer_index) override;

		virtual Texture createTexture(vector2U size, void* data, uint64_t data_size) override;
		virtual void destroyTexture(Texture texture_handle) override;

		virtual Shader createShader(string vert_data, string frag_data) override;
		virtual void destroyShader(Shader shader_handle) override;

		virtual View createView(vector2U size, FramebufferLayout& layout, CommandBufferType type) override;
		virtual void destroyView(View index) override;
		virtual void resizeView(View index, vector2U new_size) override;

		virtual void startView(View index) override;
		virtual void endView(View index) override;
		virtual void sumbitView(View index) override;
		virtual CommandBuffer* getViewCommandBuffer(View index) override;

		virtual CommandBuffer* getScreenCommandBuffer() override;

		//Utils
		virtual matrix4F getPerspectiveMatrix(Camera* camera, float aspect_ratio) override;
		virtual matrix4F getPerspectiveMatrix(Camera* camera, View view) override;

		virtual VertexBuffer getWholeScreenQuadVertex() override;
		virtual IndexBuffer getWholeScreenQuadIndex() override;

		virtual void waitTillDone() override;

	private:
		VulkanInstance* vulkan = nullptr;
		uint32_t swapchain_image_index = 0;
		uint32_t frame_index = 0;

		VertexBuffer screen_quad_vertex = nullptr;
		IndexBuffer screen_quad_index = nullptr;
	};
}