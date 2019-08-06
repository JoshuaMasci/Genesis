#pragma once

#include "Genesis/Rendering/Vulkan/VulkanDevice.hpp"

#include "Genesis/Rendering/PipelineSettings.hpp"
#include "Genesis/Rendering/Vulkan/VulkanShader.hpp"
#include "Genesis/Rendering/VertexInputDescription.hpp"

namespace Genesis
{
	class VulkanPipline
	{
	public:
		VulkanPipline(VkDevice device, VulkanShader& shader, VkRenderPass renderpass, PipelineSettings& settings, VertexInputDescription& vertex_description, VkExtent2D extent);
		virtual ~VulkanPipline();

		inline VkPipeline getPipeline() { return this->pipeline; };

	private:
		VkDevice device;
		VkPipeline pipeline;
	};
}