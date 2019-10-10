#include "VulkanPipelinePool.hpp"

using namespace Genesis;

VulkanPipelinePool::VulkanPipelinePool(VkDevice device, uint32_t thread_count)
{
	this->device = device;
	this->thread_pools.resize(thread_count);
}

VulkanPipelinePool::~VulkanPipelinePool()
{
	this->update();

	for (auto shader_pipelines : this->pipelines)
	{
		for (auto pipeline : shader_pipelines.second)
		{
			delete pipeline.second;
		}
	}
}

void VulkanPipelinePool::update()
{
	for (size_t i = 0; i < this->thread_pools.size(); i++)
	{
		while (!this->thread_pools[i].new_pipelines.empty())
		{
			PipelineAddInfo pipeline_temp = this->thread_pools[i].new_pipelines.front();
			this->pipelines[pipeline_temp.shader][pipeline_temp.pipeline_hash] = pipeline_temp.pipeline;
			this->thread_pools[i].new_pipelines.pop();
		}
		this->thread_pools[i].temp_map.clear();
	}
}

VulkanPipline* VulkanPipelinePool::getPipeline(uint32_t thread_id, VulkanShader* shader, VkRenderPass renderpass, PipelineSettings& settings, VertexInputDescription& vertex_description, VkExtent2D extent)
{
	MurmurHash2 murmur_hash;
	murmur_hash.begin();
	murmur_hash.add(renderpass);
	murmur_hash.add(settings.getHash());
	murmur_hash.add(vertex_description.getHash());
	murmur_hash.add(extent);
	uint32_t pipeline_hash = murmur_hash.end();

	if (has_value(this->pipelines, shader))
	{
		pipeline_map shader_pipelines = this->pipelines[shader];
		if (has_value(shader_pipelines, pipeline_hash))
		{
			return shader_pipelines[pipeline_hash];
		}
	}

	if (has_value(this->thread_pools[thread_id].temp_map, shader))
	{
		pipeline_map shader_pipelines = this->thread_pools[thread_id].temp_map[shader];
		if (has_value(shader_pipelines, pipeline_hash))
		{
			return shader_pipelines[pipeline_hash];
		}
	}	
	
	VulkanPipline* new_pipeline = new VulkanPipline(this->device, (*shader), renderpass, settings, vertex_description, extent);
	PipelineAddInfo pipeline_temp = {pipeline_hash, shader, new_pipeline};
	this->thread_pools[thread_id].new_pipelines.push(pipeline_temp);

	this->thread_pools[thread_id].temp_map[shader][pipeline_hash] = new_pipeline;

	return new_pipeline;
}
