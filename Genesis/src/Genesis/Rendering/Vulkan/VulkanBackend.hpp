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
		virtual void submitFrame(vector<ViewIndex> sub_views) override;

		virtual BufferIndex createBuffer(uint64_t size_bytes, BufferType type, MemoryUsage memory_usage) override;
		virtual void fillBuffer(BufferIndex buffer_index, void* data, uint64_t data_size) override;
		virtual void destroyBuffer(BufferIndex buffer_index) override;

		virtual TextureIndex createTexture(vector2U size) override;
		virtual void fillTexture(TextureIndex texture_index, void* data, uint64_t data_size) override;
		virtual void destroyTexture(TextureIndex texture_index) override;

		virtual ViewIndex createView(ViewType type, vector2U size) override;
		virtual void destroyView(ViewIndex index) override;

		virtual void startView(ViewIndex index) override;
		virtual void endView(ViewIndex index) override;
		virtual void sumbitView(ViewIndex index) override;

		virtual void drawMeshView(ViewIndex index, uint32_t thread, BufferIndex vertices_index, BufferIndex indices_index, TextureIndex texture_index, uint32_t indices_count, matrix4F mvp) override;

		//TEMP commands
		virtual void drawMeshScreen(uint32_t thread, BufferIndex vertices_index, BufferIndex indices_index, TextureIndex texture_index, uint32_t indices_count, matrix4F mvp) override;
		virtual void drawMeshScreenViewTextured(uint32_t thread, BufferIndex vertices_index, BufferIndex indices_index, ViewIndex view_index, uint32_t indices_count, matrix4F mvp) override;

		//Utils
		virtual matrix4F getPerspectiveMatrix(Camera* camera, float aspect_ratio) override;
		virtual matrix4F getPerspectiveMatrix(Camera* camera, ViewIndex view) override;

		virtual vector2U getScreenSize() override;
		virtual void waitTillDone() override;

	private:
		VulkanInstance* vulkan = nullptr;
		uint32_t swapchain_image_index = 0;
		uint32_t frame_index = 0;
	};
}