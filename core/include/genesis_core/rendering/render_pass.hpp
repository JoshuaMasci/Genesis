#pragma once

#include "image_formats.hpp"
#include "genesis_core/types/vector_math.hpp"
#include "genesis_core/types/containers.hpp"

namespace genesis
{
	typedef size_t RenderPassId;
	typedef size_t AttachmentId;

	enum class RenderPassSizeClass
	{
		Absolute,
		SwapchainRelative,
	};

	struct AttachmentCreateInfo
	{
		ImageFormat format = ImageFormat::Invalid;
		uint32_t samples = 1;
		bool persistent = true;
	};

	struct RenderPassCreateInfo
	{
		RenderPassSizeClass size_class = RenderPassSizeClass::SwapchainRelative;
		const genesis::vec2f size = vec2f(1.0f);
		std::vector<AttachmentCreateInfo> color_attachments;
		AttachmentCreateInfo depth_attachment;
	};

	struct RenderPassAttachment
	{
		AttachmentCreateInfo create_info = {};
		AttachmentId id = 0;
	};

	struct RenderPass
	{
		const RenderPassId id;
		const RenderPassSizeClass size_class;
		const genesis::vec2f size;
		const genesis::vector<RenderPassAttachment> color_attachments;
		const RenderPassAttachment depth_attachment;

		//TODO eveluate function pointer???
	};
}