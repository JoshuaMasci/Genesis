#pragma once

#include "VulkanInclude.hpp"
#include <vector>

namespace VulkanRenderer
{
	typedef size_t RenderPassId;
	typedef size_t AttachmentId;

	enum class AttachmentSizeClass
	{
		Absolute,
		SwapchainRelative,
	};

	struct AttachmentCreateInfo
	{
		VkFormat format = VK_FORMAT_UNDEFINED;
		uint32_t samples = 1;
		bool persistent = true;
	};

	struct RenderPassCreateInfo
	{
		AttachmentSizeClass size_class = AttachmentSizeClass::SwapchainRelative;
		float size_width = 1.0f;
		float size_height = 1.0f;
		std::vector<AttachmentCreateInfo> color_attachments;
		AttachmentCreateInfo depth_attachment;
	};

	struct AttachmentDescription
	{
		AttachmentCreateInfo create_info = {};
		AttachmentId id = 0;
	};

	struct RenderPassDescription
	{
		const RenderPassId id;
		const AttachmentSizeClass size_class;
		float size_width = 1.0f;
		float size_height = 1.0f;
		const std::vector<AttachmentDescription> color_attachments;
		const AttachmentDescription depth_attachment;

		//TODO eveluate function pointer???
	};
}