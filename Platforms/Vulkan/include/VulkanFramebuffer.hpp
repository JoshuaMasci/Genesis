#pragma once

#include "VulkanInclude.hpp"
#include "Genesis/RenderingBackend/RenderingTypes.hpp"
#include "VulkanDevice.hpp"
#include "VulkanDescriptorPool.hpp"

#include "Genesis/Core/Types.hpp"

namespace Genesis
{
	struct VulkanFramebufferImage
	{
		VkImage image = VK_NULL_HANDLE;
		VmaAllocation image_memory;
		VkImageView image_view = VK_NULL_HANDLE;
		VkFormat image_format = VK_FORMAT_UNDEFINED;
	};

	class VulkanFramebuffer
	{
	public:
		VulkanFramebuffer(VulkanDevice* device, VkExtent2D size, List<VkFormat>& color_formats, VkFormat depth_format, VkRenderPass render_pass);
		virtual ~VulkanFramebuffer();

		inline VkExtent2D getSize() { return this->size; };
		inline VkRenderPass getRenderPass() { return this->render_pass; };
		inline VkFramebuffer get() { return this->framebuffer; };
		inline VkImageView getImageView(uint16_t image_index) 
		{ 
			if (image_index == this->images.size())//Depth image is last
			{
				return depth_image.image_view;
			}
			return this->images[image_index].image_view; 
		};

		inline List<VkClearValue>& getClearValues() { return this->clear_values; };

	protected:
		VulkanDevice* device;
		VkExtent2D size;
		VkRenderPass render_pass;

		List<VulkanFramebufferImage> images;
		VulkanFramebufferImage depth_image;
		VkFramebuffer framebuffer;
		List<VkClearValue> clear_values;

		friend struct VulkanFramebufferSet;
	};

	struct VulkanFramebufferSet
	{
		VulkanFramebufferSet(VulkanDevice* device, VkExtent2D size, List<VkFormat>& color_formats, VkFormat depth_format, VkRenderPass render_pass, uint32_t frame_count);
		~VulkanFramebufferSet();
		List<VulkanFramebuffer*> framebuffers;

		VulkanDevice* device;
		VkExtent2D size;
		List<VkFormat> color_formats;
		VkFormat depth_format;
		VkRenderPass render_pass;

		inline VulkanFramebuffer* buildFramebuffer()
		{
			return new VulkanFramebuffer(this->device, this->size, this->color_formats, this->depth_format, this->render_pass);
		};

		inline void rebuildFramebuffer(uint32_t frame_index)
		{
			if (this->framebuffers[frame_index]->size.width != this->size.width || this->framebuffers[frame_index]->size.height != this->size.height)
			{
				delete this->framebuffers[frame_index];
				this->framebuffers[frame_index] = this->buildFramebuffer();
			}
		};
	};
}