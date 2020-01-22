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

		virtual CommandBuffer* beginFrame() = 0;
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

		virtual Framebuffer createFramebuffer(FramebufferLayout& layout, vector2U size) = 0;
		virtual void destroyFramebuffer(Framebuffer framebuffer) = 0;
		virtual void resizeFramebuffer(Framebuffer framebuffer, vector2U size) = 0;

		virtual STCommandBuffer createSTCommandBuffer() = 0;
		virtual void destroySTCommandBuffer(STCommandBuffer st_command_buffer) = 0;
		virtual CommandBuffer* beginSTCommandBuffer(STCommandBuffer st_command_buffer, Framebuffer framebuffer_target) = 0;
		virtual void endSTCommandBuffer(STCommandBuffer st_command_buffer) = 0;

		virtual MTCommandBuffer createMTCommandBuffer() = 0;
		virtual void destroyMTCommandBuffer(MTCommandBuffer mt_command_buffer) = 0;
		virtual List<CommandBuffer*>* beginMTCommandBuffer(MTCommandBuffer mt_command_buffer, Framebuffer framebuffer_target) = 0;
		virtual void endMTCommandBuffer(MTCommandBuffer mt_command_buffer) = 0;

		//Wait until all GPU proccessing is done
		virtual void waitTillDone() = 0;
	};
}