#pragma once

#include "Genesis/Rendering/Vulkan/VulkanInstance.hpp"
#include "Genesis/Rendering/Vulkan/VulkanShader.hpp"

namespace Genesis
{
	struct VertexInput
	{
		vector<VkVertexInputBindingDescription> binding_descriptions;
		vector<VkVertexInputAttributeDescription> attribute_descriptions;
	};

	class VulkanRenderPipline
	{
	public:
		VulkanRenderPipline(VulkanInstance* instance, VkPipelineLayout pipeline_layout, VkRenderPass renderpass, VulkanShader* shader, VertexInput* vertex_description);
		virtual ~VulkanRenderPipline();

	private:
		VulkanInstance* instance;
		VkPipeline pipeline;
	};
}