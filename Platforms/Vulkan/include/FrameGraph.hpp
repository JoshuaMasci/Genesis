#pragma once

#include "VulkanInclude.hpp"
#include "RenderPassDescription.hpp"

#include <vector>

namespace VulkanRenderer
{
	class FrameGraph
	{
	protected:
		friend class VulkanRenderer;
		std::vector<RenderPassDescription*> render_passes;
		AttachmentId next_attachment_id = 1;
		AttachmentId present_attachment = 0;

	public:
		~FrameGraph();

		RenderPassDescription* create_render_pass(const RenderPassCreateInfo& render_pass_info);
		void set_present_attachment(AttachmentId id);
	};
}