#pragma once

#include "Genesis/Core/VectorTypes.hpp"
#include "Genesis/RenderingBackend/RenderingTypes.hpp"

#include "Genesis/RenderingBackend/Sampler.hpp"
#include "Genesis/RenderingBackend/VertexInputDescription.hpp"
#include "Genesis/RenderingBackend/DescriptorSet.hpp"
#include "Genesis/RenderingBackend/PipelineLayout.hpp"

#include "Genesis/RenderingBackend/FramebufferLayout.hpp"
#include "Genesis/RenderingBackend/CommandBuffer.hpp"

namespace Genesis
{
	class RenderingBackend
	{
	public:
		virtual ~RenderingBackend() {};

		virtual void setScreenSize(vector2U size) = 0;
		virtual vector2U getScreenSize() = 0;

		virtual CommandBufferInterface* beginFrame() = 0;
		virtual void endFrame() = 0;

		virtual Sampler createSampler(const SamplerCreateInfo& create_info) = 0;

		virtual VertexInputDescription createVertexInputDescription(vector<VertexElementType> input_elements) = 0;
		virtual VertexInputDescription createVertexInputDescription(const VertexInputDescriptionCreateInfo& create_info) = 0;

		virtual DescriptorSetLayout createDescriptorSetLayout(const DescriptorSetLayoutCreateInfo& create_info) = 0;
		virtual DescriptorSet createDescriptorSet(const DescriptorSetCreateInfo& create_info) = 0;
		virtual void destroyDescriptorSet(DescriptorSet descriptor_set) = 0;

		virtual PipelineLayout createPipelineLayout(const PipelineLayoutCreateInfo& create_info) = 0;

		virtual ShaderModule createShaderModule(ShaderModuleCreateInfo& create_info) = 0;
		virtual void destroyShaderModule(ShaderModule shader_module) = 0;

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
		virtual CommandBufferInterface* beginSTCommandBuffer(STCommandBuffer st_command_buffer, Framebuffer framebuffer_target) = 0;
		virtual void endSTCommandBuffer(STCommandBuffer st_command_buffer) = 0;

		virtual MTCommandBuffer createMTCommandBuffer() = 0;
		virtual void destroyMTCommandBuffer(MTCommandBuffer mt_command_buffer) = 0;
		virtual List<CommandBufferInterface*>* beginMTCommandBuffer(MTCommandBuffer mt_command_buffer, Framebuffer framebuffer_target) = 0;
		virtual void endMTCommandBuffer(MTCommandBuffer mt_command_buffer) = 0;

		//Wait until all GPU proccessing is done
		virtual void waitTillDone() = 0;
	};
}