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
		VkImage image = VK_NULL_HANDLE;
		VmaAllocation image_memory;
		VkImageView image_view = VK_NULL_HANDLE;
		VkDescriptorSet image_descriptor_set;
		VkFormat image_format = VK_FORMAT_UNDEFINED;
	};

	class VulkanFramebuffer
	{
	public:
		VulkanFramebuffer(VkDevice device, VulkanAllocator* allocator, VkExtent2D size, Array<VkFormat>& color_formats, VkFormat depth_format, VkRenderPass render_pass);
		virtual ~VulkanFramebuffer();

		inline VkExtent2D getSize() { return this->size; };
		inline VkRenderPass getRenderPass() { return this->render_pass; };
		inline VkFramebuffer get() { return this->framebuffer; };
	private:
		VkDevice device;
		VkExtent2D size;
		Array<VulkanFramebufferImage> images;
		VulkanFramebufferImage depth_image;
		VkRenderPass render_pass;
		VkFramebuffer framebuffer;
		
		VulkanAllocator* allocator = nullptr;
	};

	class VulkanFramebufferLayout
	{
	public:
		VulkanFramebufferLayout(VkDevice device, Array<VkFormat>& color_formats, VkFormat depth_format);
		~VulkanFramebufferLayout();

		inline VkRenderPass getRenderPass() { return this->render_pass; };

		VulkanFramebuffer* createFramebuffer(VkExtent2D size, VulkanAllocator* allocator);

	private:
		VkDevice device;
		VkRenderPass render_pass;

		Array<VkFormat> color_formats;
		VkFormat depth_format;
	};
}