#pragma once

#include "render_pass.hpp"
#include "genesis_core/types/containers.hpp"

namespace genesis_core
{
	class FrameGraph
	{
	protected:
		friend class VulkanRenderer;
		vector<RenderPass*> render_passes;
		AttachmentId next_attachment_id = 1;
		AttachmentId present_attachment = 0;

	public:
		~FrameGraph();
		RenderPass* create_render_pass(const RenderPassCreateInfo& render_pass_info);
		void set_present_attachment(AttachmentId id);
	};
}