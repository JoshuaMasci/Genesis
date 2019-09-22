#include "VulkanPipline.hpp"

#include "Genesis/Rendering/Vulkan/VulkanShader.hpp"

using namespace Genesis;

VkFormat getVulkanType(VertexElementType type)
{
	switch (type)
	{
	case VertexElementType::float_1:
		return VK_FORMAT_R32_SFLOAT;
	case VertexElementType::float_2:
		return VK_FORMAT_R32G32_SFLOAT;
	case VertexElementType::float_3:
		return VK_FORMAT_R32G32B32_SFLOAT;
	case VertexElementType::float_4:
		return VK_FORMAT_R32G32B32A32_SFLOAT;
	case VertexElementType::unorm8_1:
		return VK_FORMAT_R8_UNORM;
	case VertexElementType::unorm8_2:
		return VK_FORMAT_R8G8_UNORM;
	case VertexElementType::unorm8_3:
		return VK_FORMAT_R8G8B8_UNORM;
	case VertexElementType::unorm8_4:
		return VK_FORMAT_R8G8B8A8_UNORM;
	case VertexElementType::uint8_1:
		return VK_FORMAT_R8_UINT;
	case VertexElementType::uint8_2:
		return VK_FORMAT_R8G8_UINT;
	case VertexElementType::uint8_3:
		return VK_FORMAT_R8G8B8_UINT;
	case VertexElementType::uint8_4:
		return VK_FORMAT_R8G8B8A8_UINT;
	case VertexElementType::uint16_1:
		return VK_FORMAT_R16_UINT;
	case VertexElementType::uint16_2:
		return VK_FORMAT_R16G16_UINT;
	case VertexElementType::uint16_3:
		return VK_FORMAT_R16G16B16_UINT;
	case VertexElementType::uint16_4:
		return VK_FORMAT_R16G16B16A16_UINT;
	case VertexElementType::uint32_1:
		return VK_FORMAT_R32_UINT;
	case VertexElementType::uint32_2:
		return VK_FORMAT_R32G32_UINT;
	case VertexElementType::uint32_3:
		return VK_FORMAT_R32G32B32_UINT;
	case VertexElementType::uint32_4:
		return VK_FORMAT_R32G32B32A32_UINT;
	default:
		return VK_FORMAT_UNDEFINED;
	}
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

VulkanPipline::VulkanPipline(VkDevice device, VulkanShader& shader, VkRenderPass renderpass, PipelineSettings& settings, VertexInputDescription& vertex_description, VkExtent2D extent)
{
	this->device = device;

	VkVertexInputBindingDescription binding_description;
	binding_description.binding = 0;
	binding_description.stride = vertex_description.getSize();
	binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	Array<VkVertexInputAttributeDescription> attribute_descriptions(vertex_description.getElementCount());
	for (size_t i = 0; i < attribute_descriptions.size(); i++)
	{
		attribute_descriptions[i].binding = 0;
		attribute_descriptions[i].location = (uint32_t) i;
		attribute_descriptions[i].format = getVulkanType(vertex_description.getElementType(i));
		attribute_descriptions[i].offset = vertex_description.getElementOffset(i);
	}

	VkPipelineVertexInputStateCreateInfo vertex_input_info = {};
	vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

	vertex_input_info.vertexBindingDescriptionCount = 1;
	vertex_input_info.pVertexBindingDescriptions = &binding_description;

	vertex_input_info.vertexAttributeDescriptionCount = (uint32_t) attribute_descriptions.size();
	vertex_input_info.pVertexAttributeDescriptions = attribute_descriptions.data();

	VkPipelineInputAssemblyStateCreateInfo input_assembly = {};
	input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	input_assembly.primitiveRestartEnable = VK_FALSE;

	VkViewport viewport = {};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)extent.width;
	viewport.height = (float)extent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor = {};
	scissor.offset = { 0, 0 };
	scissor.extent = extent;

	VkPipelineViewportStateCreateInfo viewportState = {};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.pViewports = &viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissor;

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

	Array<VkDynamicState> dynamic_states(1);
	dynamic_states[0] = VK_DYNAMIC_STATE_SCISSOR;
	//Maybe Use VK_DYNAMIC_STATE_VIEWPORT

	VkPipelineDynamicStateCreateInfo dynamic_state = {};
	dynamic_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamic_state.dynamicStateCount = (uint32_t)dynamic_states.size();
	dynamic_state.pDynamicStates = dynamic_states.data();

	vector<VkPipelineShaderStageCreateInfo> shader_stage = shader.getShaderStages();

	VkGraphicsPipelineCreateInfo pipelineInfo = {};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = (uint32_t)shader_stage.size();
	pipelineInfo.pStages = shader_stage.data();
	pipelineInfo.pVertexInputState = &vertex_input_info;
	pipelineInfo.pInputAssemblyState = &input_assembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pDepthStencilState = &depth_stencil;
	pipelineInfo.pColorBlendState = &color_blending;
	pipelineInfo.layout = shader.getPipelineLayout();
	pipelineInfo.pDynamicState = &dynamic_state;
	pipelineInfo.renderPass = renderpass;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

	if (vkCreateGraphicsPipelines(this->device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &this->pipeline) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create graphics pipeline!");
	}
}

VulkanPipline::~VulkanPipline()
{
	vkDestroyPipeline(this->device, this->pipeline, nullptr);
}