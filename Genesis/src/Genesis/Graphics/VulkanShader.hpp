#pragma once

#include "Genesis/Graphics/VulkanDevice.hpp"

namespace Genesis
{
	class VulkanShader
	{
	public:
		VulkanShader(VulkanDevice* device, string vertex_name, string fragment_name);
		~VulkanShader();

		vector<VkPipelineShaderStageCreateInfo> getShaderStages();

	private:
		VulkanDevice* device = nullptr;

		VkShaderModule vertShaderModule;
		VkShaderModule fragShaderModule;

		vector<VkPipelineShaderStageCreateInfo> shaderStages;
	};
}