#pragma once

#include "Genesis/Core/Types.hpp"
#include "Genesis/Rendering/Vulkan/VulkanInclude.hpp"
#include "Genesis/Rendering/Vulkan/VulkanDevice.hpp"
#include "Genesis/Rendering/Vulkan/VulkanPipline.hpp"

namespace Genesis
{
	typedef map<uint32_t, VulkanPipline*> pipeline_map;
	typedef map<VulkanShader*, pipeline_map> shader_pipeline_map;

	class VulkanPipelinePool
	{
	public:
		 VulkanPipelinePool(VkDevice device);
		~VulkanPipelinePool();

		VulkanPipline* getPipeline(VulkanShader* shader, VkRenderPass renderpass, PipelineSettings& settings, VertexInputDescription& vertex_description, VkExtent2D extent);

	private:
		VkDevice device = VK_NULL_HANDLE;

		shader_pipeline_map pipelines;
	};
}