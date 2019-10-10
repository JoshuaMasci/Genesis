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
		 VulkanPipelinePool(VkDevice device, uint32_t thread_count);
		~VulkanPipelinePool();

		void update();

		VulkanPipline* getPipeline(uint32_t thread_id, VulkanShader* shader, VkRenderPass renderpass, PipelineSettings& settings, VertexInputDescription& vertex_description, VkExtent2D extent);

	private:
		VkDevice device = VK_NULL_HANDLE;

		shader_pipeline_map pipelines;

		struct PipelineAddInfo
		{
			uint32_t pipeline_hash;
			VulkanShader* shader;
			VulkanPipline* pipeline;
		};

		struct ThreadPipelinePool
		{
			shader_pipeline_map temp_map;
			queue<PipelineAddInfo> new_pipelines;
		};

		Array<ThreadPipelinePool> thread_pools;
	};
}