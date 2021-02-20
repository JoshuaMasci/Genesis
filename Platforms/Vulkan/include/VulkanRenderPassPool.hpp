#pragma once

#include <mutex>

#include "VulkanInclude.hpp"
#include "VulkanDevice.hpp"
#include "Genesis/RenderingBackend/RenderPass.hpp"

namespace Genesis
{
	class VulkanRenderPassPool
	{
	public:
		VulkanRenderPassPool(VkDevice device);
		~VulkanRenderPassPool();

		//VkRenderPass getRenderPass(uint32_t hash, vector<VkFormat>& color_formats, VkFormat depth_format);
		//VkRenderPass getRenderPass(vector<VkAttachmentDescription>& attachments, VkAttachmentDescription depth_attachment);
		VkRenderPass getRenderPass(const RenderPassCreateInfo& create_info);

	private:
		std::mutex render_pass_map_lock;

		VkDevice device = VK_NULL_HANDLE;

		map<uint32_t, VkRenderPass> render_passes;
	};
}