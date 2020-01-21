#pragma once

#include "Genesis/Rendering/Vulkan/VulkanInclude.hpp"
#include "Genesis/Rendering/RenderingTypes.hpp"
#include "Genesis/Rendering/Vulkan/VulkanDevice.hpp"
#include "Genesis/Rendering/Vulkan/VulkanDescriptorPool.hpp"

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

	private:
		VulkanDevice* device;
		VkExtent2D size;
		VkRenderPass render_pass;

		List<VulkanFramebufferImage> images;
		VulkanFramebufferImage depth_image;
		VkFramebuffer framebuffer;
		List<VkClearValue> clear_values;
	};

	class VulkanFramebufferSet
	{
	public:
		VulkanFramebufferSet(VulkanDevice* device, VkExtent2D size, List<VkFormat>& color_formats, VkFormat depth_format, VkRenderPass render_pass, uint32_t frame_count);
		virtual ~VulkanFramebufferSet();

		void updateFramebuffer(uint32_t frame);

		VulkanFramebuffer* getFramebuffer(uint32_t frame) { return this->framebuffers[frame]; };

		inline void setSize(VkExtent2D new_size) { this->size = new_size; };
		inline VkExtent2D getSize() { this->size; };

	private:
		VulkanDevice* device;
		VkExtent2D size;
		List<VkFormat> color_formats;
		VkFormat depth_format;
		VkRenderPass render_pass;

		List<VulkanFramebuffer*> framebuffers;
	};
}