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

		virtual VertexBuffer createVertexBuffer(void* data, uint64_t data_size, VertexInputDescription& vertex_input_description, MemoryUsage memory_usage = MemoryUsage::GPU_Only) = 0;
		virtual void destroyVertexBuffer(VertexBuffer vertex_buffer) = 0;

		virtual IndexBuffer createIndexBuffer(void* data, uint64_t data_size, IndexType type, MemoryUsage memory_usage = MemoryUsage::GPU_Only) = 0;
		virtual void destroyIndexBuffer(IndexBuffer index_buffer) = 0;

		virtual UniformBuffer createUniformBuffer(uint64_t data_size, MemoryUsage memory_usage = MemoryUsage::CPU_Visable) = 0;
		virtual void destroyUniformBuffer(UniformBuffer uniform_buffer) = 0;
		virtual void setUniform(UniformBuffer uniform_buffer, void* data, uint64_t data_size) = 0;

		virtual Texture createTexture(vector2U size, void* data, uint64_t data_size) = 0;
		virtual void destroyTexture(Texture texture) = 0;

		virtual Shader createShader(string& vert_data, string& frag_data) = 0;
		virtual void destroyShader(Shader shader) = 0;

		/*virtual Framebuffer createFramebuffer(FramebufferLayout& layout, vector2U size) = 0;
		virtual void destroyFramebuffer(Framebuffer frame_buffer) = 0;
		virtual void resizeFramebuffer(Framebuffer frame_buffer, vector2U size) = 0;

		virtual CommandBuffer* createCommandBuffer() = 0;
		virtual void destroyCommandBuffer(CommandBuffer* command_buffer) = 0;
		virtual void beginCommandBuffer(CommandBuffer* command_buffer, Framebuffer target) = 0;
		virtual void endCommandBuffer(CommandBuffer* command_buffer) = 0;
		virtual void submitCommandBuffer(CommandBuffer* command_buffer) = 0;*/

		virtual View createView(FramebufferLayout& layout, vector2U size) = 0;
		virtual void destroyView(View view) = 0;
		virtual void resizeView(View view, vector2U size) = 0;
		virtual CommandBuffer* beginView(View view) = 0;
		virtual void endView(View view) = 0;

		//Wait until all GPU proccessing is done
		virtual void waitTillDone() = 0;
	};
}