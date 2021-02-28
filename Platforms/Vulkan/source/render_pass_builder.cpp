#include "vulkan_renderer/render_pass_builder.hpp"

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

	RenderPassBuilder::RenderPassBuilder(Device* device)
	{
		this->device = device;
	}

	RenderPassBuilder::~RenderPassBuilder()
	{
		this->flush_frame();
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