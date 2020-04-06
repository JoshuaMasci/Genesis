#pragma once

#include "Genesis/Core/VectorTypes.hpp"
#include "Genesis/RenderingBackend/RenderingTypes.hpp"

#include "Genesis/RenderingBackend/Sampler.hpp"
#include "Genesis/RenderingBackend/VertexInputDescription.hpp"
#include "Genesis/RenderingBackend/DescriptorSet.hpp"
#include "Genesis/RenderingBackend/PipelineLayout.hpp"
#include "Genesis/RenderingBackend/RenderPass.hpp"

#include "Genesis/RenderingBackend/FramebufferLayout.hpp"
#include "Genesis/RenderingBackend/CommandBuffer.hpp"

#include "Genesis/FrameGraph/FrameGraph.hpp"

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

		virtual VertexInputDescription createVertexInputDescription(const VertexInputDescriptionCreateInfo& create_info) = 0;

		virtual DescriptorSetLayout createDescriptorSetLayout(const DescriptorSetLayoutCreateInfo& create_info) = 0;
		virtual DescriptorSet createDescriptorSet(const DescriptorSetCreateInfo& create_info) = 0;
		virtual void destroyDescriptorSet(DescriptorSet descriptor_set) = 0;

		virtual PipelineLayout createPipelineLayout(const PipelineLayoutCreateInfo& create_info) = 0;

		virtual RenderPass createRenderPass(const RenderPassCreateInfo& create_info) = 0;

		virtual ShaderModule createShaderModule(ShaderModuleCreateInfo& create_info) = 0;
		virtual void destroyShaderModule(ShaderModule shader_module) = 0;

		virtual StaticBuffer createStaticBuffer(void* data, uint64_t data_size, BufferUsage buffer_usage, MemoryType memory_usage = MemoryType::GPU_Only) = 0;
		virtual void destroyStaticBuffer(StaticBuffer buffer) = 0;

		virtual DynamicBuffer createDynamicBuffer(uint64_t data_size, BufferUsage buffer_usage, MemoryType memory_usage = MemoryType::CPU_Visable) = 0;
		virtual void destroyDynamicBuffer(DynamicBuffer buffer) = 0;
		virtual void writeDynamicBuffer(DynamicBuffer buffer, void* data, uint64_t data_size) = 0;

		virtual Texture createTexture(vector2U size, void* data, uint64_t data_size) = 0;
		virtual void destroyTexture(Texture texture) = 0;

		//Test Functions
		virtual void submitFrameGraph(FrameGraph* render_graph) = 0;

		//Wait until all GPU proccessing is done
		virtual void waitTillDone() = 0;
	};
}