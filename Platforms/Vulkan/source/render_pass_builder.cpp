#include "vulkan_renderer/render_pass_builder.hpp"

#include "genesis_core/rendering/vertex_elements.hpp"

namespace genesis
{
	VkFormat get_color_format(ColorFormat format)
	{
		switch (format)
		{
		case ColorFormat::RGBA_8_Srgb:
			return VK_FORMAT_R8G8B8A8_SRGB;
		case ColorFormat::RGBA_8_Unorm:
			return VK_FORMAT_R8G8B8A8_UNORM;
		case ColorFormat::RGBA_8_Snorm:
			return VK_FORMAT_R8G8B8A8_SNORM;
		case ColorFormat::R_16_Float:
			return VK_FORMAT_R16_SFLOAT;
		case ColorFormat::RG_16_Float:
			return VK_FORMAT_R16G16_SFLOAT;
		case ColorFormat::RGB_16_Float:
			return VK_FORMAT_R16G16B16_SFLOAT;
		case ColorFormat::RGBA_16_Float:
			return VK_FORMAT_R16G16B16A16_SFLOAT;
		case ColorFormat::R_32_Float:
			return VK_FORMAT_R32_SFLOAT;
		case ColorFormat::RG_32_Float:
			return VK_FORMAT_R16G16_SFLOAT;
		case ColorFormat::RGB_32_Float:
			return VK_FORMAT_R16G16B16_SFLOAT;
		case ColorFormat::RGBA_32_Float:
			return VK_FORMAT_R16G16B16A16_SFLOAT;
		default:
			return VK_FORMAT_UNDEFINED;
		}
	};

	VkFormat get_depth_format(DepthFormat format)
	{
		switch (format)
		{
		case DepthFormat::S_8_Uint:
			return VK_FORMAT_S8_UINT;
		case DepthFormat::D_16_Unorm:
			return VK_FORMAT_D16_UNORM;
		case DepthFormat::D_24_Unorm_S8:
			return VK_FORMAT_D24_UNORM_S8_UINT;
		case DepthFormat::D_32_Float:
			return VK_FORMAT_D32_SFLOAT;
		default:
			return VK_FORMAT_UNDEFINED;
		}
	}

	VkFormat get_vertex_attribute_type(VertexElementType type)
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

	vector<Image*> RenderPassBuilder::get_color_attachments(const vec2u& size, const vector<ColorRenderPassAttachment>& color_attachments)
	{
		vector<Image*> new_color_attachments(color_attachments.size());
		for (size_t i = 0; i < color_attachments.size(); i++)
		{
			new_color_attachments[i] = get_color_attachment(size, color_attachments[i]);
		}
		return new_color_attachments;
	}

	Image* RenderPassBuilder::get_color_attachment(const vec2u& size, const ColorRenderPassAttachment& color_attachment)
	{
		VkImageCreateInfo image_info = {};
		image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		image_info.imageType = VK_IMAGE_TYPE_2D;
		image_info.extent.width = size.x;
		image_info.extent.height = size.y;
		image_info.extent.depth = 1;
		image_info.mipLevels = 1;
		image_info.arrayLayers = 1;
		image_info.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		image_info.format = get_color_format(color_attachment.create_info.format);
		image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
		image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		image_info.samples = (VkSampleCountFlagBits)color_attachment.create_info.samples;
		image_info.flags = 0;

		Image* image = new Image(this->device, image_info, VMA_MEMORY_USAGE_GPU_ONLY);
		this->attachments[color_attachment.id] = image;
		return image;
	}

	Image* RenderPassBuilder::get_depth_attachment(const vec2u& size, const DepthRenderPassAttachment& depth_attachment)
	{
		if (depth_attachment.id != 0)
		{
			VkImageCreateInfo image_info = {};
			image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			image_info.imageType = VK_IMAGE_TYPE_2D;
			image_info.extent.width = size.x;
			image_info.extent.height = size.y;
			image_info.extent.depth = 1;
			image_info.mipLevels = 1;
			image_info.arrayLayers = 1;
			image_info.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
			image_info.format = get_depth_format(depth_attachment.create_info.format);
			image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
			image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			image_info.samples = (VkSampleCountFlagBits)depth_attachment.create_info.samples;
			image_info.flags = 0;

			Image* image = new Image(this->device, image_info, VMA_MEMORY_USAGE_GPU_ONLY);
			this->attachments[depth_attachment.id] = image;
			return image;
		}

		return nullptr;
	}

	VkRenderPass RenderPassBuilder::get_vulkan_render_pass(RenderPass* node)
	{
		vector<VkAttachmentDescription> attachment_descriptions;
		vector<VkAttachmentReference> color_attachment_references;

		for (auto color_attachment : node->color_attachments)
		{
			VkAttachmentDescription color_description = {};
			color_description.format = get_color_format(color_attachment.create_info.format);
			color_description.samples = (VkSampleCountFlagBits)color_attachment.create_info.samples;
			color_description.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			color_description.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			color_description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			color_description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			color_description.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			color_description.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

			VkAttachmentReference color_reference = {};
			color_reference.attachment = (uint32_t)attachment_descriptions.size();
			color_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

			attachment_descriptions.push_back(color_description);
			color_attachment_references.push_back(color_reference);
		}

		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = (uint32_t)color_attachment_references.size();
		subpass.pColorAttachments = color_attachment_references.data();

		if (node->depth_attachment.id != 0)
		{
			uint32_t depth_position = (uint32_t)attachment_descriptions.size();

			VkAttachmentReference depth_reference = {};
			depth_reference.attachment = depth_position;
			depth_reference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			subpass.pDepthStencilAttachment = &depth_reference;

			VkAttachmentDescription depth_description = {};
			depth_description.format = get_depth_format(node->depth_attachment.create_info.format);
			depth_description.samples = (VkSampleCountFlagBits)node->depth_attachment.create_info.samples;
			depth_description.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			depth_description.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			depth_description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			depth_description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
			depth_description.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			depth_description.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

			attachment_descriptions.push_back(depth_description);
		}
		else
		{
			subpass.pDepthStencilAttachment = VK_NULL_HANDLE;
		}

		VkSubpassDependency dependencies[2];
		dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
		dependencies[0].dstSubpass = 0;
		dependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
		dependencies[0].dstStageMask = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
		dependencies[0].srcAccessMask = 0;
		dependencies[0].dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

		dependencies[1].srcSubpass = 0;
		dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
		dependencies[1].srcStageMask = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
		dependencies[1].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		dependencies[1].srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		dependencies[1].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

		VkRenderPassCreateInfo vk_render_pass_info = {};
		vk_render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		vk_render_pass_info.attachmentCount = (uint32_t)attachment_descriptions.size();
		vk_render_pass_info.pAttachments = attachment_descriptions.data();
		vk_render_pass_info.subpassCount = 1;
		vk_render_pass_info.pSubpasses = &subpass;
		vk_render_pass_info.dependencyCount = 2;
		vk_render_pass_info.pDependencies = dependencies;

		VkRenderPass render_pass = VK_NULL_HANDLE;
		VK_ASSERT(vkCreateRenderPass(this->device->get(), &vk_render_pass_info, nullptr, &render_pass));
		this->render_passes.push_back(render_pass);
		return render_pass;
	}

	VkFramebuffer RenderPassBuilder::get_vulkan_framebuffer(const vec2u& size, VkRenderPass render_pass, const vector<Image*>& color_attachments, const Image* depth_attachment)
	{
		vector<VkImageView> image_views;
		image_views.reserve(color_attachments.size() + (depth_attachment ? 1 : 0));

		for (const Image* color_attachment : color_attachments)
		{
			image_views.push_back(color_attachment->get_view());
		}

		if (depth_attachment)
		{
			image_views.push_back(depth_attachment->get_view());
		}

		VkFramebufferCreateInfo framebuffer_info = {};
		framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebuffer_info.renderPass = render_pass;

		framebuffer_info.attachmentCount = (uint32_t)image_views.size();
		framebuffer_info.pAttachments = image_views.data();

		framebuffer_info.width = size.x;
		framebuffer_info.height = size.y;
		framebuffer_info.layers = 1;

		VkFramebuffer framebuffer = VK_NULL_HANDLE;
		VK_ASSERT(vkCreateFramebuffer(this->device->get(), &framebuffer_info, nullptr, &framebuffer));
		this->frame_buffers.push_back(framebuffer);
		return framebuffer;
	}

	struct VertexInputState
	{
		VkVertexInputBindingDescription vertex_binding = {};
		vector<VkVertexInputAttributeDescription> vertex_attributes;
	};

	VertexInputState create_vertex_input_state(const vector<VertexElementType>& vertex_elements)
	{
		VertexInputState vertex_input_state = {};
		vertex_input_state.vertex_attributes.resize(vertex_elements.size());

		uint32_t i = 0;
		uint32_t offset = 0;
		for (auto& vertex_attribute : vertex_input_state.vertex_attributes)
		{
			VertexElementType vertex_element = vertex_elements[i];
			vertex_attribute.binding = 0;
			vertex_attribute.location = i;
			vertex_attribute.format = get_vertex_attribute_type(vertex_element);
			vertex_attribute.offset = offset;
			offset += VertexElementTypeInfo::get_element_size_bytes(vertex_element);
			i++;
		}

		vertex_input_state.vertex_binding.binding = 0;
		vertex_input_state.vertex_binding.stride = offset;
		vertex_input_state.vertex_binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return vertex_input_state;
	}

	VkBlendFactor get_blend_factor(BlendFactor blend_factor)
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

	VkPipeline RenderPassBuilder::create_pipeline(const vec2u& size, VkRenderPass render_pass, const PipelineCreateInfo& create_info)
	{
		VkVertexInputBindingDescription vertex_input_binding = {};

		VertexInputState vertex_input_state = create_vertex_input_state(create_info.vertex_elements);
		VkPipelineVertexInputStateCreateInfo vertex_input_info = {};
		vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertex_input_info.pVertexBindingDescriptions = &vertex_input_state.vertex_binding;
		vertex_input_info.vertexBindingDescriptionCount = 1;
		vertex_input_info.pVertexAttributeDescriptions = vertex_input_state.vertex_attributes.data();
		vertex_input_info.vertexAttributeDescriptionCount = (uint32_t)vertex_input_state.vertex_attributes.size();

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
		switch (create_info.cull_mode)
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
		switch (create_info.depth_test)
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
		switch (create_info.depth_op)
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
		if (create_info.blend_op != BlendOp::None)
		{
			VkBlendFactor src_factor = get_blend_factor(create_info.src_factor);
			VkBlendFactor dst_factor = get_blend_factor(create_info.dst_factor);
			VkBlendOp blend_op;
			switch (create_info.blend_op)
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

		vector<VkPipelineShaderStageCreateInfo> shader_stages;
		if (create_info.vertex_shader != nullptr)
		{
			VkPipelineShaderStageCreateInfo shader_stage = {};
			shader_stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			shader_stage.stage = VK_SHADER_STAGE_VERTEX_BIT;
			shader_stage.module = *(VkShaderModule*)create_info.vertex_shader;
			shader_stage.pName = "main";
			shader_stages.push_back(shader_stage);
		}

		if (create_info.fragment_shader != nullptr)
		{
			VkPipelineShaderStageCreateInfo shader_stage = {};
			shader_stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			shader_stage.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
			shader_stage.module = *(VkShaderModule*)create_info.fragment_shader;
			shader_stage.pName = "main";
			shader_stages.push_back(shader_stage);
		}

		VkGraphicsPipelineCreateInfo pipeline_info = {};
		pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipeline_info.stageCount = (uint32_t)shader_stages.size();
		pipeline_info.pStages = shader_stages.data();
		pipeline_info.pVertexInputState = &vertex_input_info;
		pipeline_info.pInputAssemblyState = &input_assembly;
		pipeline_info.pViewportState = &viewport_state;
		pipeline_info.pRasterizationState = &rasterizer;
		pipeline_info.pMultisampleState = &multisampling;
		pipeline_info.pDepthStencilState = &depth_stencil;
		pipeline_info.pColorBlendState = &color_blending;
		pipeline_info.layout = bindless_pipeline_layout;
		pipeline_info.pDynamicState = &dynamic_state;
		pipeline_info.renderPass = render_pass;
		pipeline_info.subpass = 0;
		pipeline_info.basePipelineHandle = VK_NULL_HANDLE;

		VkPipeline pipeline = VK_NULL_HANDLE;
		VK_ASSERT(vkCreateGraphicsPipelines(this->device->get(), VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &pipeline));
		return pipeline;
	}

	RenderPassBuilder::RenderPassBuilder(Device* device, VkDescriptorSetLayout layout)
	{
		this->device = device;

		VkPipelineLayoutCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		create_info.pPushConstantRanges = nullptr;
		create_info.pushConstantRangeCount = 0;
		create_info.pSetLayouts = &layout;
		create_info.setLayoutCount = 1;
		VK_ASSERT(vkCreatePipelineLayout(this->device->get(), &create_info, nullptr, &this->bindless_pipeline_layout));
	}

	RenderPassBuilder::~RenderPassBuilder()
	{
		this->flush_frame();

		vkDestroyPipelineLayout(this->device->get(), this->bindless_pipeline_layout, nullptr);
	}

	RenderPassInfo RenderPassBuilder::create_render_pass(const vec2u& swapchain_size, RenderPass* render_pass)
	{
		vec2u render_pass_size = vec2u(0);
		if (render_pass->size_class == RenderPassSizeClass::SwapchainRelative)
		{
			render_pass_size.x = (uint32_t)(render_pass->size.x * swapchain_size.x);
			render_pass_size.y = (uint32_t)(render_pass->size.y * swapchain_size.y);
		}
		else if (render_pass->size_class == RenderPassSizeClass::Absolute)
		{
			render_pass_size.x = (uint32_t)render_pass->size.x;
			render_pass_size.y = (uint32_t)render_pass->size.y;
		}

		RenderPassInfo render_pass_info = {};
		render_pass_info.node = render_pass;
		render_pass_info.color_attachments = this->get_color_attachments(render_pass_size, render_pass->color_attachments);
		render_pass_info.depth_attachment = this->get_depth_attachment(render_pass_size, render_pass->depth_attachment);
		render_pass_info.renderpass = this->get_vulkan_render_pass(render_pass);
		render_pass_info.framebuffer = this->get_vulkan_framebuffer(render_pass_size, render_pass_info.renderpass, render_pass_info.color_attachments, render_pass_info.depth_attachment);

		for (auto& pipeline_create_info : render_pass->create_pipelines)
		{
			VkPipeline pipeline = this->create_pipeline(render_pass_size, render_pass_info.renderpass, pipeline_create_info);
			vkDestroyPipeline(this->device->get(), pipeline, nullptr);
		}

		return render_pass_info;
	}

	void RenderPassBuilder::flush_frame()
	{
		for (auto framebuffer : this->frame_buffers)
		{
			vkDestroyFramebuffer(this->device->get(), framebuffer, nullptr);
		}
		this->frame_buffers.clear();

		for (auto attachment : this->attachments)
		{
			delete attachment.second;
		}
		this->attachments.clear();

		for (auto render_pass : this->render_passes)
		{
			vkDestroyRenderPass(this->device->get(), render_pass, nullptr);
		}
		this->render_passes.clear();
	}
}