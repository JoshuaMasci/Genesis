#pragma once

#include "Genesis/Rendering/Vulkan/VulkanInclude.hpp"
#include "Genesis/Rendering/RenderingTypes.hpp"
#include "Genesis/Rendering/Vulkan/VulkanDevice.hpp"
#include "Genesis/Rendering/Vulkan/VulkanAllocator.hpp"

#include "Genesis/Core/Types.hpp"

namespace Genesis
{
	struct VulkanFramebufferImage
	{
		VkImage image;
		VmaAllocation image_memory;
		VkImageView image_view;
		VkDescriptorSet image_descriptor_set;
		VkFormat image_format;
	};

	class VulkanFramebuffer
	{
	public:
		VulkanFramebuffer(VkDevice device, VulkanAllocator* allocator, VkExtent2D size, Array<VkFormat>& color_formats, VkFormat depth_format);
		virtual ~VulkanFramebuffer();

	private:
		VkDevice device;
		VkExtent2D size;
		Array<VulkanFramebufferImage> images;
		VkFramebuffer framebuffer;

		VulkanAllocator* allocator = nullptr;
	};

	class VulkanFramebufferLayout
	{
	public:
		VulkanFramebufferLayout(VkDevice device, Array<VkFormat>& color_formats, VkFormat depth_format);
		~VulkanFramebufferLayout();

		inline VkRenderPass getRenderPass() { return this->render_pass; };

		void createFramebuffer(VkExtent2D size);

	private:
		VkDevice device;
		VkRenderPass render_pass;

		Array<VkFormat> color_formats;
		VkFormat depth_format;
	};
}