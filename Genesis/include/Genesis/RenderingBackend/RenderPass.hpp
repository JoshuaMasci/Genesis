#pragma once

#include "Genesis/RenderingBackend/RenderingTypes.hpp"

namespace Genesis
{
	typedef void* RenderPass;

	struct RenderPassAttachment
	{
		ImageFormat format;
		IamgeSamples samples;
		bool should_load;
		bool should_clear;
		bool should_store;
	};

	struct RenderPassSubpass
	{
		uint32_t* color_attachment_index;
		uint32_t color_attachments_count;

		uint32_t* depth_attachment_index;

		uint32_t* input_attachment_index;
		uint32_t input_attachments_count;

		//TODO
		//Resolve Attachments
		//Preserve Attachments
	};

	struct RenderPassCreateInfo
	{
		RenderPassAttachment* depth_attachment;

		RenderPassAttachment* color_attachments;
		uint32_t color_attachments_count;

		RenderPassSubpass* subpasses;
		uint32_t subpasses_count;
	};

}