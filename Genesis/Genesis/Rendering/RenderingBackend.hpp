#pragma once

#include "Genesis/Core/VectorTypes.hpp"
#include "Genesis/Rendering/RenderingTypes.hpp"
#include "Genesis/Rendering/VertexInputDescription.hpp"
#include "Genesis/Rendering/FramebufferLayout.hpp"
#include "Genesis/Rendering/CommandBuffer.hpp"
#include "Genesis/Rendering/Sampler.hpp"

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

		virtual Sampler createSampler(SamplerCreateInfo& create_info) = 0;
		virtual VertexInputDescription createVertexInputDescription(vector<VertexElementType> input_elements) = 0;

		virtual StaticBuffer createStaticBuffer(void* data, uint64_t data_size, BufferUsage buffer_usage, MemoryType memory_usage = MemoryType::GPU_Only) = 0;
		virtual void destroyStaticBuffer(StaticBuffer buffer) = 0;

		virtual DynamicBuffer createDynamicBuffer(uint64_t data_size, BufferUsage buffer_usage, MemoryType memory_usage = MemoryType::CPU_Visable) = 0;
		virtual void destroyDynamicBuffer(DynamicBuffer buffer) = 0;
		virtual void writeDynamicBuffer(DynamicBuffer buffer, void* data, uint64_t data_size) = 0;

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