#include "VulkanShader.hpp"

#include <fstream>

using namespace Genesis;

static std::vector<char> readFile(const string& filename) 
{
	std::ifstream file(filename, std::ios::ate | std::ios::binary);

	if (!file.is_open()) 
	 {
		throw std::runtime_error("failed to open file!");
	}

	size_t fileSize = (size_t)file.tellg();
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);

	file.close();

	return buffer;
}

VkShaderModule createShaderModule(VulkanDevice* device, const std::vector<char>& code)
{
	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	VkShaderModule shaderModule;
	if (vkCreateShaderModule(device->getDevice(), &createInfo, nullptr, &shaderModule) != VK_SUCCESS) 
	{
		throw std::runtime_error("failed to create shader module!");
	}

	return shaderModule;
}

VulkanShader::VulkanShader(VulkanDevice* device, string vertex_name, string fragment_name)
{
	this->device = device;

	auto vertShaderCode = readFile(vertex_name);
	auto fragShaderCode = readFile(fragment_name);

	vertShaderModule = createShaderModule(this->device, vertShaderCode);
	fragShaderModule = createShaderModule(this->device, fragShaderCode);

	VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = vertShaderModule;
	vertShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = fragShaderModule;
	fragShaderStageInfo.pName = "main";

	shaderStages = { vertShaderStageInfo, fragShaderStageInfo };
}

VulkanShader::~VulkanShader()
{
	vkDestroyShaderModule(this->device->getDevice(), this->fragShaderModule, nullptr);
	vkDestroyShaderModule(this->device->getDevice(), this->vertShaderModule, nullptr);
}

vector<VkPipelineShaderStageCreateInfo> Genesis::VulkanShader::getShaderStages()
{
	return this->shaderStages;
}
