#include "VulkanRenderPassManager.hpp"

using namespace Genesis;

VulkanRenderPassManager::VulkanRenderPassManager(VkDevice device)
{
	this->device = device;
}

VulkanRenderPassManager::~VulkanRenderPassManager()
{
	for(auto render_pass : this->render_passes)
	{
		vkDestroyRenderPass(this->device, render_pass.second, nullptr);
	}
}

VkRenderPass VulkanRenderPassManager::getRenderPass(uint32_t hash, Array<VkFormat>& color_formats, VkFormat depth_format)
{
	if (this->render_passes.find(hash) != this->render_passes.end())
	{
		return this->render_passes[hash];
	}

	vector<VkAttachmentDescription> attachment_descriptions;
	vector<VkAttachmentReference> color_attachment_references;

	for (uint16_t i = 0; i < color_formats.size(); i++)
	{
		VkAttachmentDescription color_description = {};
		color_description.format = color_formats[i];
		color_description.samples = VK_SAMPLE_COUNT_1_BIT;
		color_description.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		color_description.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		color_description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		color_description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		color_description.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		color_description.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		VkAttachmentReference color_reference = {};
		color_reference.attachment = i;
		color_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		attachment_descriptions.push_back(color_description);
		color_attachment_references.push_back(color_reference);
	}

	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = (uint32_t)color_attachment_references.size();
	subpass.pColorAttachments = color_attachment_references.data();

	if (depth_format != VK_FORMAT_UNDEFINED)
	{
		uint32_t depth_pos = (uint32_t)attachment_descriptions.size();

		VkAttachmentReference depth_reference = {};
		depth_reference.attachment = depth_pos;
		depth_reference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		subpass.pDepthStencilAttachment = &depth_reference;

		VkAttachmentDescription depth_description = {};
		depth_description.format = depth_format;
		depth_description.samples = VK_SAMPLE_COUNT_1_BIT;
		depth_description.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depth_description.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depth_description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		depth_description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depth_description.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		depth_description.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		attachment_descriptions.push_back(depth_description);
	}
	else
	{
		subpass.pDepthStencilAttachment = VK_NULL_HANDLE;
	}

	//Use subpass dependencies for layout transitions
	Array<VkSubpassDependency> dependencies(2);

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

	VkRenderPassCreateInfo render_pass_info = {};
	render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;

	render_pass_info.attachmentCount = (uint32_t)attachment_descriptions.size();
	render_pass_info.pAttachments = attachment_descriptions.data();
	render_pass_info.subpassCount = 1;
	render_pass_info.pSubpasses = &subpass;
	render_pass_info.dependencyCount = (uint32_t)dependencies.size();
	render_pass_info.pDependencies = dependencies.data();

	VkRenderPass render_pass;

	if (vkCreateRenderPass(device, &render_pass_info, nullptr, &render_pass) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create render pass!");
	}

	this->render_passes[hash] = render_pass;

	return render_pass;
}
