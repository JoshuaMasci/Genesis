#pragma once

#include "Genesis/Core/Types.hpp"
#include "VulkanInclude.hpp"
#include "VulkanShader.hpp"

#include "Genesis/Rendering/PipelineSettings.hpp"
#include "Genesis/Rendering/VertexInputDescription.hpp"

namespace Genesis
{
	typedef map<uint32_t, VkPipeline> pipeline_map;
	typedef map<VulkanShader*, pipeline_map> shader_pipeline_map;

	class VulkanThreadPipelinePool;

	class VulkanPipelinePool
	{

	public:
		 VulkanPipelinePool(VkDevice device);
		~VulkanPipelinePool();

		void update();

	protected:
		VkDevice device = VK_NULL_HANDLE;

		shader_pipeline_map pipelines;

		struct PipelineAddInfo
		{
			uint32_t pipeline_hash;
			VulkanShader* shader;
			VkPipeline pipeline;
		};

		vector<VulkanThreadPipelinePool*> thread_pools;

		friend class VulkanThreadPipelinePool;
	};

	class VulkanThreadPipelinePool
	{
	public:
		VulkanThreadPipelinePool(VkDevice device, VulkanPipelinePool* main_pool);
		~VulkanThreadPipelinePool();

		VkPipeline getPipeline(VulkanShader* shader, VkRenderPass renderpass, PipelineSettings& settings, VertexInputDescription* vertex_description);

	protected:
		VkDevice device = VK_NULL_HANDLE;
		VulkanPipelinePool* main_pool = nullptr;
		size_t main_pool_index;

		shader_pipeline_map temp_map;
		queue<VulkanPipelinePool::PipelineAddInfo> new_pipelines;

		VkPipeline createPipeline(VulkanShader* shader, VkRenderPass renderpass, PipelineSettings& settings, VertexInputDescription* vertex_description);

		friend class VulkanPipelinePool;
	};
}