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

		virtual ShaderHandle createShader(string vert_data, string frag_data) override;
		virtual void destroyShader(ShaderHandle shader_handle) override;

		virtual BufferHandle createBuffer(uint64_t size_bytes, BufferType type, MemoryUsage memory_usage) override;
		virtual void fillBuffer(BufferHandle buffer_handle, void* data, uint64_t data_size) override;
		virtual void destroyBuffer(BufferHandle buffer_handle) override;

		virtual TextureHandle createTexture(vector2U size) override;
		virtual void fillTexture(TextureHandle texture_handle, void* data, uint64_t data_size) override;
		virtual void destroyTexture(TextureHandle texture_handle) override;

		virtual ViewHandle createView(ViewType type, vector2U size) override;
		virtual void destroyView(ViewHandle index) override;

		virtual void startView(ViewHandle index) override;
		virtual void endView(ViewHandle index) override;
		virtual void sumbitView(ViewHandle index) override;

		virtual void drawMeshView(ViewHandle index, uint32_t thread, BufferHandle vertices_handle, BufferHandle indices_handle, uint32_t indices_count, TextureHandle texture_handle, matrix4F mvp) override;

		//TEMP commands
		virtual void drawMeshScreen(uint32_t thread, BufferHandle vertices_handle, BufferHandle indices_handle, uint32_t indices_count, TextureHandle texture_handle, matrix4F mvp) override;
		virtual void drawMeshScreenViewTextured(uint32_t thread, BufferHandle vertices_handle, BufferHandle indices_handle, uint32_t indices_count, ViewHandle view_handle, matrix4F mvp) override;

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