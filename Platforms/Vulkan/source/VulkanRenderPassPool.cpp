#include "VulkanRenderPassPool.hpp"

#include "Genesis/Debug/Assert.hpp"
#include "Genesis/Core/MurmurHash2.hpp"

using namespace Genesis;

VulkanRenderPassPool::VulkanRenderPassPool(VkDevice device)
{
	this->device = device;
}

VulkanRenderPassPool::~VulkanRenderPassPool()
{
	for(auto render_pass : this->render_passes)
	{
		vkDestroyRenderPass(this->device, render_pass.second, nullptr);
	}
}

uint32_t createHash(const RenderPassCreateInfo& create_info)
{
	MurmurHash2 hash;
	hash.addData((uint8_t*)create_info.color_attachments, (uint32_t)(sizeof(RenderPassAttachment) * create_info.color_attachments_count));

	if (create_info.depth_attachment != nullptr)
	{
		hash.add(*create_info.depth_attachment);
	}

	for (size_t i = 0; i < create_info.subpasses_count; i++)
	{
		RenderPassSubpass& subpass = create_info.subpasses[i];

		hash.addData((uint8_t*)subpass.color_attachment_index, (uint32_t)(sizeof(uint32_t) * subpass.color_attachments_count));

		if (subpass.depth_attachment_index != nullptr)
		{
			hash.add(*subpass.depth_attachment_index);
		}

		if (subpass.input_attachments_count != 0)
		{
			hash.addData((uint8_t*)subpass.input_attachment_index, (uint32_t)(sizeof(uint32_t) * subpass.input_attachments_count));
		}
	}

	return hash.end();
}

VkRenderPass createRenderPass(const RenderPassCreateInfo& create_info)
{
	vector<VkAttachmentDescription> attachment_descriptions(create_info.color_attachments_count);
	for (size_t i = 0; i < attachment_descriptions.size(); i++)
	{
		VkAttachmentDescription& attachment_description = attachment_descriptions[i];
		//attachment_description.format = //create_info.color_attachments[i];
	}


	return VK_NULL_HANDLE;
}

VkRenderPass Genesis::VulkanRenderPassPool::getRenderPass(const RenderPassCreateInfo& create_info)
{
	uint32_t hash_value = createHash(create_info);
	this->render_pass_map_lock.lock();

	if (!has_value(this->render_passes, hash_value))
	{
		this->render_passes[hash_value] = createRenderPass(create_info);
	}

	VkRenderPass render_pass;
	render_pass = this->render_passes[hash_value];
	this->render_pass_map_lock.unlock();
	return render_pass;
}
