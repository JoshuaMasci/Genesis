#pragma once

#include "Genesis/RenderGraph/RenderPass.hpp"
#include "Genesis/Core/VectorTypes.hpp"

namespace Genesis
{
	struct ImageAttachmentCreateInfo
	{
		ImageFormat format;
		vector3U size;

		uint32_t mip_levels;
		uint32_t array_layers;
		uint32_t samples;
	};

	class FramebufferRenderPass : public RenderPass
	{
	public:
		FramebufferRenderPass(vector2U size);

		void setBuildFunction(std::function<void(CommandBufferInterface*, vector2U)> function);

		void addColorOutput(ImageAttachmentCreateInfo attachment_info);
		void addDepthStencilOutput(ImageAttachmentCreateInfo attachment_info);

		FramebufferAttachement getColorAttachement(uint32_t color_index);
		FramebufferAttachement getDepthStencilAttachement();

		virtual void build(CommandBufferInterface* command_buffer) override;

	protected:
		const vector2U framebuffer_size;

		std::function<void(CommandBufferInterface*, vector2U)> build_function;

		struct ImageColorAttachment
		{
			ImageAttachmentCreateInfo attachment_info;
			ShaderStage stages_read;
			FramebufferAttachement handle;
		};
		vector<ImageColorAttachment> color_attachments;
		
		bool has_depth_stencil = false;
		ImageColorAttachment depth_stencil_attachment;
	};
}