#include "genesis_core/rendering/frame_graph.hpp"

namespace genesis_core
{
	FrameGraph::~FrameGraph()
	{
		for (auto* render_pass : this->render_passes)
		{
			delete render_pass;
		}
	}

	RenderPass* FrameGraph::create_render_pass(const RenderPassCreateInfo& render_pass_info)
	{
		std::vector<RenderPassAttachment> color_attachments(render_pass_info.color_attachments.size());
		RenderPassAttachment depth_attachment = { render_pass_info.depth_attachment, 0 };

		for (size_t i = 0; i < color_attachments.size(); i++)
		{
			color_attachments[i] = { render_pass_info.color_attachments[i], this->next_attachment_id++ };
		}

		if (render_pass_info.depth_attachment.format != ImageFormat::Invalid)
		{
			depth_attachment.id = this->next_attachment_id++;
		}

		RenderPass* render_pass = new RenderPass{ this->render_passes.size() + 1, render_pass_info.size_class, render_pass_info.size, color_attachments, depth_attachment };
		this->render_passes.push_back(render_pass);
		return render_pass;
	}

	void FrameGraph::set_present_attachment(AttachmentId id)
	{
		this->present_attachment = id;
	}
}