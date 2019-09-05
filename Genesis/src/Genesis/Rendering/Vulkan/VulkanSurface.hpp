#pragma once

#include "Genesis/Core/Types.hpp"
#include "Genesis/Rendering/Vulkan/VulkanInclude.hpp"

namespace Genesis
{
	class VulkanSurface
	{
	public:
		VulkanSurface(VkInstance instance, VulkanDevice* device, VkSurfaceKHR surface)
		{
			this->instance = instance;
			this->device = device;
			this->surface = surface;

			VkPhysicalDevice physical_device = device->getPhysicalDevice();

			{
				vector<VkSurfaceFormatKHR> formats;
				uint32_t format_count = 0;
				vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &format_count, nullptr);
				if (format_count != 0)
				{
					formats.resize(format_count);
					vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &format_count, formats.data());
				}
				//Force this format
				//TODO allow others
				this->format = { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
			}

			{
				vector<VkPresentModeKHR> present_modes;
				VkPresentModeKHR best_mode = VK_PRESENT_MODE_FIFO_KHR; //All systesm support this one
				uint32_t present_count = 0;
				vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &present_count, nullptr);
				if (present_count != 0)
				{
					present_modes.resize(present_count);
					vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &present_count, present_modes.data());
					for (const auto& availablePresentMode : present_modes)
					{
						if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
						{
							best_mode = availablePresentMode; //The mode we want
							break;
						}
						else if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR)
						{
							best_mode = availablePresentMode; //Second best, better than VK_PRESENT_MODE_FIFO_KHR
						}
					}
				}
				this->present_mode = best_mode;
			}

			vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, this->surface, &this->capabilities);

			this->image_count = this->capabilities.minImageCount + 1;
			if (this->capabilities.maxImageCount > 0 && this->image_count > this->capabilities.maxImageCount)
			{
				this->image_count = this->capabilities.maxImageCount;
			}

			//choose depth format
			{
				this->depth_format = VK_FORMAT_UNDEFINED;

				vector<VkFormat> depth_formats =
				{
					VK_FORMAT_D32_SFLOAT_S8_UINT, //Best
					VK_FORMAT_D32_SFLOAT,
					VK_FORMAT_D24_UNORM_S8_UINT,
					VK_FORMAT_D16_UNORM_S8_UINT,
					VK_FORMAT_D16_UNORM //Worst
				};

				for (VkFormat format : depth_formats)
				{
					VkFormatProperties format_props;
					vkGetPhysicalDeviceFormatProperties(physical_device, format, &format_props);
					// Format must support depth stencil attachment for optimal tiling
					if (format_props.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
					{
						this->depth_format = format;
						break;
					}
				}

				if (this->depth_format == VK_FORMAT_UNDEFINED)
				{
					throw std::runtime_error("failed to find depth format!");
				}
			}


			//Build Screen Renderpass
			{
				VkAttachmentDescription colorAttachment = {};
				colorAttachment.format = this->format.format;
				colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
				colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
				colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
				colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
				colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
				colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
				colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

				VkAttachmentDescription depthAttachment = {};
				depthAttachment.format = this->depth_format;
				depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
				depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
				depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
				depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
				depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
				depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
				depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

				VkAttachmentReference colorAttachmentRef = {};
				colorAttachmentRef.attachment = 0;
				colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

				VkAttachmentReference depthAttachmentRef = {};
				depthAttachmentRef.attachment = 1;
				depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

				VkSubpassDescription subpass = {};
				subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
				subpass.colorAttachmentCount = 1;
				subpass.pColorAttachments = &colorAttachmentRef;
				subpass.pDepthStencilAttachment = &depthAttachmentRef;

				VkSubpassDependency dependency = {};
				dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
				dependency.dstSubpass = 0;
				dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
				dependency.srcAccessMask = 0;
				dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
				dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

				vector<VkAttachmentDescription> attachments = { colorAttachment, depthAttachment };
				VkRenderPassCreateInfo renderPassInfo = {};
				renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
				renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
				renderPassInfo.pAttachments = attachments.data();
				renderPassInfo.subpassCount = 1;
				renderPassInfo.pSubpasses = &subpass;
				renderPassInfo.dependencyCount = 1;
				renderPassInfo.pDependencies = &dependency;

				if (vkCreateRenderPass(this->device->get(), &renderPassInfo, nullptr, &this->screen_render_pass) != VK_SUCCESS)
				{
					throw std::runtime_error("failed to create screen render pass!");
				}
			}
		};

		~VulkanSurface()
		{
			//Don't bother destroying it, It will be destroyed when the device is destroyed
			//It will crash if destroy is called since device will already be destroyed by this point
			//vkDestroyRenderPass(this->device->get(), this->screen_render_pass, nullptr);
			vkDestroySurfaceKHR(this->instance, this->surface, nullptr);
		};

		inline VkSurfaceKHR get() { return this->surface; };

		inline VkSurfaceCapabilitiesKHR getCapabilities() { return this->capabilities; };
		inline VkSurfaceFormatKHR getFormat() { return this->format; };
		inline VkPresentModeKHR getPresentMode() { return this->present_mode; };
		inline uint32_t getImageCount() { return this->image_count; };

		inline VkFormat getDepthFormat() { return this->depth_format; };

		inline VkRenderPass getRenderPass() { return this->screen_render_pass; };

	private:
		VkInstance instance;
		VulkanDevice* device;

		VkSurfaceKHR surface;

		VkSurfaceCapabilitiesKHR capabilities;
		VkSurfaceFormatKHR format;
		VkPresentModeKHR present_mode;
		uint32_t image_count;

		VkFormat depth_format;

		VkRenderPass screen_render_pass;
	};
}