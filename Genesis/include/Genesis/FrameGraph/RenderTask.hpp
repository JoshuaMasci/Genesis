#pragma once

#include "Genesis/FrameGraph/FrameTask.hpp"
#include "Genesis/Core/VectorTypes.hpp"

namespace Genesis
{
	struct FramebufferAttachmentDescription
	{
		ImageFormat format;
		uint32_t samples;
	};

	class RenderTask;
	struct FramebufferAttachment
	{
		RenderTask* parent;
		uint32_t usage_counts;

		ImageFormat format;
		uint32_t samples;

		Texture resolved_texture;
	};

	enum class RenderTaskSize
	{
		Absolute,
		SwapchainRelative,
		InputRelative
	};

	class RenderTask : public FrameTask
	{
	public:
		RenderTask(RenderTaskSize size_type, vector2F size);

		void setBuildFunction(std::function<void(CommandBufferInterface*, vector2U)> function);

		void addColorOutput(FramebufferAttachmentDescription color_description);
		void addDepthStencilOutput(FramebufferAttachmentDescription depth_description);

		virtual void build(CommandBufferInterface* command_buffer) override;

		vector<FramebufferAttachment>& getColorAttachments() { return this->color_attachments; };
		FramebufferAttachment* getColorAttachment(uint32_t index) { return &this->color_attachments[index]; };

		bool hasDepthStencilAttachment() { return this->has_depth_stencil; };
		FramebufferAttachment* getDepthStencilAttachment() { return &this->depth_stencil_attachment; };

	protected:

		friend class FrameGraph;

		const RenderTaskSize size_type;
		const vector2U size;

		std::function<void(CommandBufferInterface*, vector2U)> build_function;

		vector<FramebufferAttachment> color_attachments;

		bool has_depth_stencil = false;
		FramebufferAttachment depth_stencil_attachment;
	};
}