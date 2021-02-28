#pragma once

#include "vulkan_include.hpp"
#include "genesis_core/rendering/render_pass.hpp"
#include "device.hpp"
#include "image.hpp"


namespace genesis
{
	struct RenderPassInfo
	{
		RenderPass* node = nullptr;
		vector<Image*> color_attachments;
		Image* depth_attachment = nullptr;
		VkRenderPass renderpass = VK_NULL_HANDLE;
		VkFramebuffer framebuffer = VK_NULL_HANDLE;
	};

	class RenderPassBuilder
	{
	protected:
		Device* device = nullptr;

		flat_hash_map<AttachmentId, Image*> attachments;
		vector<VkRenderPass> render_passes;
		vector<VkFramebuffer> frame_buffers;

		vector<Image*> get_color_attachments(const vec2u& size, const vector<ColorRenderPassAttachment>& color_attachments);
		Image* get_color_attachment(const vec2u& size, const ColorRenderPassAttachment& color_attachment);
		Image* get_depth_attachment(const vec2u& size, const DepthRenderPassAttachment& depth_attachment);
		VkRenderPass get_vulkan_render_pass(RenderPass* node);
		VkFramebuffer get_vulkan_framebuffer(const vec2u& size, VkRenderPass render_pass, const vector<Image*>& color_attachments, const Image* depth_attachment);

	public:
		RenderPassBuilder(Device* device);
		~RenderPassBuilder();

		RenderPassInfo create_render_pass(const vec2u& swapchain_size, RenderPass* render_pass);

		void flush_frame();
	};
}