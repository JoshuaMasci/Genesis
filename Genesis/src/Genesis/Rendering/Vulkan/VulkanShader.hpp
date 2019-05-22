#pragma once

#include "Genesis/Core/Types.hpp"
#include "Genesis/Rendering/Vulkan/VulkanInclude.hpp"

namespace Genesis
{
	class VulkanShader
	{
	public:
		VulkanShader(VkDevice device, string vertex_name, string fragment_name);
		~VulkanShader();

		vector<VkPipelineShaderStageCreateInfo> getShaderStages();

	private:
		VkDevice device;

		VkShaderModule vertShaderModule;
		VkShaderModule fragShaderModule;

		vector<VkPipelineShaderStageCreateInfo> shaderStages;
	};
}