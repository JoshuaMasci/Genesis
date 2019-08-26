#include "VulkanPipelineManager.hpp"

using namespace Genesis;

VulkanPipelineManager::VulkanPipelineManager(VkDevice device)
{
	this->device = device;
}

VulkanPipelineManager::~VulkanPipelineManager()
{
	for (auto shader_pipelines : this->pipelines)
	{
		for (auto pipeline : shader_pipelines.second)
		{
			delete pipeline.second;
		}
	}
}

VulkanPipline* VulkanPipelineManager::getPipeline(VulkanShader* shader, VkRenderPass renderpass, PipelineSettings& settings, VertexInputDescription& vertex_description, VkExtent2D extent)
{
	MurmurHash2 murmur_hash;
	murmur_hash.begin();
	murmur_hash.add(renderpass);
	murmur_hash.add(settings.getHash());
	murmur_hash.add(vertex_description.getHash());
	murmur_hash.add(extent);
	uint32_t pipeline_hash = murmur_hash.end();

	if (this->pipelines.find(shader) != this->pipelines.end())
	{
		pipeline_map shader_pipelines = this->pipelines[shader];
		if (shader_pipelines.find(pipeline_hash) != shader_pipelines.end())
		{
			return shader_pipelines[pipeline_hash];
		}
	}

	printf("Building new pipeline with hash: %u\n", pipeline_hash);

	VulkanPipline* new_pipeline = new VulkanPipline(this->device, (*shader), renderpass, settings, vertex_description, extent);

	this->pipelines[shader][pipeline_hash] = new_pipeline;

	return new_pipeline;
}
