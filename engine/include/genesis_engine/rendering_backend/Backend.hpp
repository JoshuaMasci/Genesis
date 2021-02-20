#pragma once

#include "genesis_engine/renderingBackend/RenderingTypes.hpp"

#include "genesis_engine/renderingBackend/Sampler.hpp"
#include "genesis_engine/renderingBackend/VertexInputDescription.hpp"
#include "genesis_engine/renderingBackend/DescriptorSet.hpp"
#include "genesis_engine/renderingBackend/PipelineLayout.hpp"
#include "genesis_engine/renderingBackend/RenderPass.hpp"

#include "genesis_engine/renderingBackend/FramebufferLayout.hpp"
#include "genesis_engine/renderingBackend/CommandBuffer.hpp"

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

		virtual Texture2D createTexture(vector2U size, void* data, uint64_t data_size) = 0;
		virtual void destroyTexture(Texture2D texture) = 0;

		//Wait until all GPU proccessing is done
		virtual void waitTillDone() = 0;
	};
}