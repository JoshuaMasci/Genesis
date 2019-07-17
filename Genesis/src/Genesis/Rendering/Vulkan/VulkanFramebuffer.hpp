#pragma once

#include "Genesis/Rendering/Vulkan/VulkanInclude.hpp"
#include "Genesis/Rendering/RenderingTypes.hpp"
#include "Genesis/Rendering/Vulkan/VulkanDevice.hpp"
#include "Genesis/Rendering/Vulkan/VulkanAllocator.hpp"
#include "Genesis/Rendering/Vulkan/VulkanDescriptorPool.hpp"

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
		VulkanFramebuffer(VkDevice device, VulkanAllocator* allocator, VulkanDescriptorPool* descriptor_pool, VkExtent2D size, Array<VkFormat>& color_formats, VkFormat depth_format, VkRenderPass render_pass, VkSampler sampler);
		virtual ~VulkanFramebuffer();

		inline VkExtent2D getSize() { return this->size; };
		inline VkRenderPass getRenderPass() { return this->render_pass; };
		inline VkFramebuffer get() { return this->framebuffer; };

		//TEMP
		inline VkImageView getImageView(uint16_t i) { return this->images[i].image_view; };
		inline VkDescriptorSet getImageDescriptor(uint16_t i) { return this->images[i].image_descriptor_set; };

	private:
		VkDevice device;
		VkExtent2D size;
		Array<VulkanFramebufferImage> images;
		VulkanFramebufferImage depth_image;
		VkRenderPass render_pass;
		VkFramebuffer framebuffer;
		
		VulkanAllocator* allocator = nullptr;
		VulkanDescriptorPool* descriptor_pool = nullptr;
	};

	class VulkanFramebufferLayout
	{
	public:
		VulkanFramebufferLayout(VkDevice device, Array<VkFormat>& color_formats, VkFormat depth_format, VkSampler sampler);
		~VulkanFramebufferLayout();

		inline VkRenderPass getRenderPass() { return this->render_pass; };

		VulkanFramebuffer* createFramebuffer(VkExtent2D size, VulkanAllocator* allocator, VulkanDescriptorPool* descriptor_pool);

	private:
		VkDevice device;
		VkRenderPass render_pass;

		Array<VkFormat> color_formats;
		VkFormat depth_format;
		VkSampler image_sampler;
	};
}