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
		virtual void submitFrame(vector<ViewHandle> sub_views) override;

		virtual VertexBufferHandle createVertexBuffer(void* data, uint64_t data_size, VertexInputDescription& vertex_input_description, MemoryUsage memory_usage = MemoryUsage::GPU_Only) override;
		virtual void destroyVertexBuffer(VertexBufferHandle vertex_buffer_index) override;

		virtual IndexBufferHandle createIndexBuffer(void* indices, uint32_t indices_count, IndexType type, MemoryUsage memory_usage = MemoryUsage::GPU_Only) override;
		virtual void destroyIndexBuffer(IndexBufferHandle index_buffer_index) override;

		virtual TextureHandle createTexture(vector2U size, void* data, uint64_t data_size) override;
		virtual void destroyTexture(TextureHandle texture_handle) override;

		virtual ShaderHandle createShader(string vert_data, string frag_data) override;
		virtual void destroyShader(ShaderHandle shader_handle) override;

		virtual ViewHandle createView(vector2U size, FramebufferLayout& layout, CommandBufferType type) override;
		virtual void destroyView(ViewHandle index) override;
		virtual void resizeView(ViewHandle index, vector2U new_size) override;

		virtual void startView(ViewHandle index) override;
		virtual void endView(ViewHandle index) override;
		virtual void sumbitView(ViewHandle index) override;
		virtual CommandBuffer* getViewCommandBuffer(ViewHandle index) override;

		virtual CommandBuffer* getScreenCommandBuffer() override;

		//Utils
		virtual matrix4F getPerspectiveMatrix(Camera* camera, float aspect_ratio) override;
		virtual matrix4F getPerspectiveMatrix(Camera* camera, ViewHandle view) override;

		virtual VertexBufferHandle getWholeScreenQuadVertex() override;
		virtual IndexBufferHandle getWholeScreenQuadIndex() override;

		virtual void waitTillDone() override;

	private:
		VulkanInstance* vulkan = nullptr;
		uint32_t swapchain_image_index = 0;
		uint32_t frame_index = 0;

		VertexBufferHandle screen_quad_vertex = nullptr;
		IndexBufferHandle screen_quad_index = nullptr;
	};
}