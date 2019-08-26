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

		virtual bool beginFrame() override;
		virtual void endFrame() override;
		virtual void submitFrame(vector<ViewHandle> sub_views) override;

		virtual VertexBufferHandle createVertexBuffer(void* data, uint64_t data_size, VertexInputDescription& vertex_input_description) override;
		virtual void destroyVertexBuffer(VertexBufferHandle vertex_buffer_index) override;

		virtual IndexBufferHandle createIndexBuffer(void* data, uint64_t data_size, uint32_t indices_count) override;
		virtual void destroyIndexBuffer(IndexBufferHandle index_buffer_index) override;

		virtual UniformBufferHandle createUniformBuffer(string uniform_name, uint64_t uniform_bytes) override;
		virtual void fillUniformBuffer(UniformBufferHandle uniform_buffer_index, void* data, uint64_t data_size) override;
		virtual void destroyUniformBuffer(UniformBufferHandle uniform_buffer_index) override;

		virtual TextureHandle createTexture(vector2U size, void* data, uint64_t data_size) override;
		virtual void destroyTexture(TextureHandle texture_handle) override;

		virtual ShaderHandle createShader(string vert_data, string frag_data) override;
		virtual void destroyShader(ShaderHandle shader_handle) override;

		virtual ViewHandle createView(ViewType type, vector2U size) override;
		virtual void destroyView(ViewHandle index) override;

		virtual void startView(ViewHandle index) override;
		virtual void endView(ViewHandle index) override;
		virtual void sumbitView(ViewHandle index) override;

		virtual void tempDrawScreen(VertexBufferHandle vertices_handle, IndexBufferHandle indices_handle, ShaderHandle shader_handle, TextureHandle texture_handle, UniformBufferHandle uniform_handle) override;

		//Utils
		virtual matrix4F getPerspectiveMatrix(Camera* camera, float aspect_ratio) override;
		virtual matrix4F getPerspectiveMatrix(Camera* camera, ViewHandle view) override;

		virtual vector2U getScreenSize() override;
		virtual void waitTillDone() override;

	private:
		VulkanInstance* vulkan = nullptr;
		uint32_t swapchain_image_index = 0;
		uint32_t frame_index = 0;
	};
}