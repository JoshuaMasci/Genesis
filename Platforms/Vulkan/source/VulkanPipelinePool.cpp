#include "VulkanPipelinePool.hpp"

#include "Genesis/Debug/Assert.hpp"

using namespace Genesis;

VulkanPipelinePool::VulkanPipelinePool(VkDevice device)
{
	this->device = device;
}

VulkanPipelinePool::~VulkanPipelinePool()
{
	this->update();

	for (auto shader_pipelines : this->pipelines)
	{
		for (auto pipeline : shader_pipelines.second)
		{
			vkDestroyPipeline(this->device, pipeline.second, nullptr);
		}
	}
}

void VulkanPipelinePool::update()
{
	for (size_t i = 0; i < this->thread_pools.size(); i++)
	{
		if (this->thread_pools[i] != nullptr)
		{
			while (!this->thread_pools[i]->new_pipelines.empty())
			{
				PipelineAddInfo pipeline_temp = this->thread_pools[i]->new_pipelines.front();
				this->pipelines[pipeline_temp.shader][pipeline_temp.pipeline_hash] = pipeline_temp.pipeline;
				this->thread_pools[i]->new_pipelines.pop();
			}
			this->thread_pools[i]->temp_map.clear();
		}
	}
}

VulkanThreadPipelinePool::VulkanThreadPipelinePool(VkDevice device, VulkanPipelinePool* main_pool)
{
	this->device = device;
	this->main_pool = main_pool;
	this->main_pool_index = this->main_pool->thread_pools.size();
	this->main_pool->thread_pools.push_back(this);
}

VulkanThreadPipelinePool::~VulkanThreadPipelinePool()
{
	this->main_pool->thread_pools[this->main_pool_index] = nullptr;
}

VkPipeline VulkanThreadPipelinePool::getPipeline(VulkanShader* shader, VkRenderPass renderpass, PipelineSettings& settings, VulkanVertexInputDescription* vertex_description)
{
	GENESIS_ENGINE_ASSERT_ERROR((shader != nullptr), "Shader Null");
	GENESIS_ENGINE_ASSERT_ERROR((renderpass != VK_NULL_HANDLE), "RenderPass Null");
	GENESIS_ENGINE_ASSERT_ERROR((vertex_description != nullptr), "VertexInputDescription Null");

	MurmurHash2 murmur_hash;
	murmur_hash.begin();
	murmur_hash.add(renderpass);
	murmur_hash.add(settings.getHash());
	murmur_hash.add(vertex_description);
	uint32_t pipeline_hash = murmur_hash.end();

	if (has_value(this->temp_map, shader))
	{
		pipeline_map& shader_pipelines = this->temp_map[shader];
		if (has_value(shader_pipelines, pipeline_hash))
		{
			return shader_pipelines[pipeline_hash];
		}
	}

	if (has_value(this->main_pool->pipelines, shader))
	{
		pipeline_map& shader_pipelines = this->main_pool->pipelines[shader];
		if (has_value(shader_pipelines, pipeline_hash))
		{
			return shader_pipelines[pipeline_hash];
		}
	}

	VkPipeline new_pipeline = this->createPipeline(shader, renderpass, settings, vertex_description);
	VulkanPipelinePool::PipelineAddInfo pipeline_temp = { pipeline_hash, shader, new_pipeline };
	this->new_pipelines.push(pipeline_temp);
	this->temp_map[shader][pipeline_hash] = new_pipeline;

	return new_pipeline;
}

VkBlendFactor getBlendFactor(BlendFactor blend_factor)
{
	switch (blend_factor)
	{
	case BlendFactor::Zero:
		return VK_BLEND_FACTOR_ZERO;
	case BlendFactor::One:
		return VK_BLEND_FACTOR_ONE;
	case BlendFactor::Color_Src:
		return VK_BLEND_FACTOR_SRC_COLOR;
	case BlendFactor::One_Minus_Color_Src:
		return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
	case BlendFactor::Color_Dst:
		return VK_BLEND_FACTOR_DST_COLOR;
	case BlendFactor::One_Minus_Color_Dst:
		return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
	case BlendFactor::Alpha_Src:
		return VK_BLEND_FACTOR_SRC_ALPHA;
	case BlendFactor::One_Minus_Alpha_Src:
		return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	case BlendFactor::Alpha_Dst:
		return VK_BLEND_FACTOR_DST_ALPHA;
	case BlendFactor::One_Minus_Alpha_Dst:
		return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
	default:
		return VK_BLEND_FACTOR_ZERO;//Invalid
	}
}

VkPipeline VulkanThreadPipelinePool::createPipeline(VulkanShader* shader, VkRenderPass renderpass, PipelineSettings& settings, VulkanVertexInputDescription* vertex_description)
{
	VkPipelineVertexInputStateCreateInfo vertex_input_info = {};
	vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertex_input_info.vertexBindingDescriptionCount = 1;
	vertex_input_info.pVertexBindingDescriptions = &vertex_description->binding_description;
	vertex_input_info.vertexAttributeDescriptionCount = (uint32_t)vertex_description->attribute_descriptions.size();
	vertex_input_info.pVertexAttributeDescriptions = vertex_description->attribute_descriptions.data();

	VkPipelineInputAssemblyStateCreateInfo input_assembly = {};
	input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	input_assembly.primitiveRestartEnable = VK_FALSE;

	VkPipelineViewportStateCreateInfo viewport_state = {};
	viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewport_state.viewportCount = 1;
	viewport_state.scissorCount = 1;
	viewport_state.flags = 0;

	VkPipelineRasterizationStateCreateInfo rasterizer = {};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;

	switch (settings.cull_mode)
	{
	case CullMode::None:
		rasterizer.cullMode = VK_CULL_MODE_NONE;
		break;
	case CullMode::Front:
		rasterizer.cullMode = VK_CULL_MODE_FRONT_BIT;
		break;
	case CullMode::Back:
		rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
		break;
	case CullMode::All:
		rasterizer.cullMode = VK_CULL_MODE_FRONT_AND_BACK;
		break;
	}
	rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE; //Front Face is always Counter Clockwise
	rasterizer.depthBiasEnable = VK_FALSE;

	VkPipelineMultisampleStateCreateInfo multisampling = {};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	VkPipelineDepthStencilStateCreateInfo depth_stencil = {};
	depth_stencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;

	switch (settings.depth_test)
	{
	case DepthTest::None:
		depth_stencil.depthTestEnable = VK_FALSE;
		depth_stencil.depthWriteEnable = VK_FALSE;
		break;
	case DepthTest::Test_Only:
		depth_stencil.depthTestEnable = VK_TRUE;
		depth_stencil.depthWriteEnable = VK_FALSE;
		break;
	case DepthTest::Test_And_Write:
		depth_stencil.depthTestEnable = VK_TRUE;
		depth_stencil.depthWriteEnable = VK_TRUE;
		break;
	}

	switch (settings.depth_op)
	{
	case DepthOp::Never:
		depth_stencil.depthCompareOp = VK_COMPARE_OP_NEVER;
		break;
	case DepthOp::Less:
		depth_stencil.depthCompareOp = VK_COMPARE_OP_LESS;
		break;
	case DepthOp::Equal:
		depth_stencil.depthCompareOp = VK_COMPARE_OP_EQUAL;
		break;
	case DepthOp::Less_Equal:
		depth_stencil.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
		break;
	case DepthOp::Greater:
		depth_stencil.depthCompareOp = VK_COMPARE_OP_GREATER;
		break;
	case DepthOp::Not_Equal:
		depth_stencil.depthCompareOp = VK_COMPARE_OP_NOT_EQUAL;
		break;
	case DepthOp::Greater_Equal:
		depth_stencil.depthCompareOp = VK_COMPARE_OP_GREATER_OR_EQUAL;
		break;
	case DepthOp::Always:
		depth_stencil.depthCompareOp = VK_COMPARE_OP_ALWAYS;
		break;
	}

	depth_stencil.depthBoundsTestEnable = VK_FALSE;
	depth_stencil.stencilTestEnable = VK_FALSE;

	VkPipelineColorBlendAttachmentState color_blend_attachment = {};
	color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

	if (settings.blend_op != BlendOp::None)
	{
		VkBlendFactor src_factor = getBlendFactor(settings.src_factor);
		VkBlendFactor dst_factor = getBlendFactor(settings.dst_factor);
		VkBlendOp blend_op;
		switch (settings.blend_op)
		{
		case BlendOp::Add:
			blend_op = VK_BLEND_OP_ADD;
			break;
		case BlendOp::Subtract:
			blend_op = VK_BLEND_OP_SUBTRACT;
			break;
		case BlendOp::Reverse_Subtract:
			blend_op = VK_BLEND_OP_REVERSE_SUBTRACT;
			break;
		case BlendOp::Min:
			blend_op = VK_BLEND_OP_MIN;
			break;
		case BlendOp::Max:
			blend_op = VK_BLEND_OP_MAX;
			break;
		}

		color_blend_attachment.blendEnable = VK_TRUE;
		color_blend_attachment.srcColorBlendFactor = src_factor;
		color_blend_attachment.srcAlphaBlendFactor = src_factor;

		color_blend_attachment.dstColorBlendFactor = dst_factor;
		color_blend_attachment.dstAlphaBlendFactor = dst_factor;

		color_blend_attachment.colorBlendOp = blend_op;
		color_blend_attachment.alphaBlendOp = blend_op;
	}
	else
	{
		color_blend_attachment.blendEnable = VK_FALSE;
	}

	VkPipelineColorBlendStateCreateInfo color_blending = {};
	color_blending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	color_blending.logicOpEnable = VK_FALSE;
	color_blending.logicOp = VK_LOGIC_OP_COPY;
	color_blending.attachmentCount = 1;
	color_blending.pAttachments = &color_blend_attachment;
	color_blending.blendConstants[0] = 0.0f;
	color_blending.blendConstants[1] = 0.0f;
	color_blending.blendConstants[2] = 0.0f;
	color_blending.blendConstants[3] = 0.0f;

	VkDynamicState dynamic_states[2];
	dynamic_states[0] = VK_DYNAMIC_STATE_SCISSOR;
	dynamic_states[1] = VK_DYNAMIC_STATE_VIEWPORT;

	VkPipelineDynamicStateCreateInfo dynamic_state = {};
	dynamic_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamic_state.dynamicStateCount = 2;
	dynamic_state.pDynamicStates = dynamic_states;

	vector<VkPipelineShaderStageCreateInfo> shader_stage = shader->getShaderStages();

	VkGraphicsPipelineCreateInfo pipeline_info = {};
	pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipeline_info.stageCount = (uint32_t)shader_stage.size();
	pipeline_info.pStages = shader_stage.data();
	pipeline_info.pVertexInputState = &vertex_input_info;
	pipeline_info.pInputAssemblyState = &input_assembly;
	pipeline_info.pViewportState = &viewport_state;
	pipeline_info.pRasterizationState = &rasterizer;
	pipeline_info.pMultisampleState = &multisampling;
	pipeline_info.pDepthStencilState = &depth_stencil;
	pipeline_info.pColorBlendState = &color_blending;
	pipeline_info.layout = shader->getPipelineLayout();
	pipeline_info.pDynamicState = &dynamic_state;
	pipeline_info.renderPass = renderpass;
	pipeline_info.subpass = 0;
	pipeline_info.basePipelineHandle = VK_NULL_HANDLE;

	VkPipeline pipeline = VK_NULL_HANDLE;
	GENESIS_ENGINE_ASSERT_ERROR((vkCreateGraphicsPipelines(this->device, VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &pipeline) == VK_SUCCESS), "failed to create graphics pipeline");
	return pipeline;
}