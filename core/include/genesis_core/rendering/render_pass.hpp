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

	struct ColorAttachmentInfo
	{
		ColorFormat format = ColorFormat::Invalid;
		uint32_t samples = 1;
	};

	struct DepthAttachmentInfo
	{
		DepthFormat format = DepthFormat::Invalid;
		uint32_t samples = 1;
	};

	struct RenderPassCreateInfo
	{
		RenderPassSizeClass size_class = RenderPassSizeClass::SwapchainRelative;
		const genesis::vec2f size = vec2f(1.0f);
		vector<ColorAttachmentInfo> color_attachments;
		optional<DepthAttachmentInfo> depth_attachment;
	};

	struct ColorRenderPassAttachment
	{
		ColorAttachmentInfo create_info = {};
		AttachmentId id = 0;
	};

	struct DepthRenderPassAttachment
	{
		DepthAttachmentInfo create_info = {};
		AttachmentId id = 0;
	};

	struct RenderPass
	{
		const RenderPassId id;
		const RenderPassSizeClass size_class;
		const vec2f size;
		const vector<ColorRenderPassAttachment> color_attachments;
		const DepthRenderPassAttachment depth_attachment;

		//TODO eveluate function pointer???
	};
}