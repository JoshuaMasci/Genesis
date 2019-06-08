#pragma once

#include "Genesis/Rendering/Vulkan/VulkanDevice.hpp"
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
		VulkanRenderPipline(VkDevice device, VkPipelineLayout pipeline_layout, VkRenderPass renderpass, string shader_path, VertexInput* vertex_description, VkExtent2D extent);
		virtual ~VulkanRenderPipline();

		inline VkPipeline getPipeline() { return this->pipeline; };

	private:
		VkDevice device;
		VkPipeline pipeline;
	};
}