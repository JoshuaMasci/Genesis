#include "VulkanSwapchain.hpp"

#include "Genesis/Rendering/Vulkan/VulkanQueueFamily.hpp"
#include <algorithm>

using namespace Genesis;

VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, VkExtent2D current_extent)
{
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
	{
		return capabilities.currentExtent;
	}
	else
	{
		VkExtent2D actualExtent = current_extent;
		actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
		actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

		return actualExtent;
	}
}

Genesis::VulkanSwapchain::VulkanSwapchain(VulkanDevice* device, VkExtent2D surface_size, VkSurfaceKHR surface)
{
	this->device = device;
	this->surface = surface;

	this->current = this->createSwapchain(this->device->getPhysicalDevice(), surface_size, this->surface, VK_NULL_HANDLE);
}

VulkanSwapchain::~VulkanSwapchain()
{
	if (!this->swapchain_invalid)
	{
		this->destroySwapchain(this->current);
	}
	else
	{
		this->destroySwapchain(this->old);
	}
}

void VulkanSwapchain::invalidateSwapchain()
{
	this->swapchain_invalid = true;
	this->old = current;
}

void VulkanSwapchain::recreateSwapchain(VkExtent2D surface_size)
{
	VkSurfaceCapabilitiesKHR capabilities;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(this->device->getPhysicalDevice(), this->surface, &capabilities);
	if (capabilities.maxImageExtent.width <= 1, capabilities.maxImageExtent.height <= 1)
	{
		return; //Can't recreate swapchain
	}

	this->swapchain_invalid = false;
	this->current = this->createSwapchain(this->device->getPhysicalDevice(), surface_size, this->surface, this->old.swapchain);
	this->destroySwapchain(this->old);
}

uint32_t VulkanSwapchain::getNextImage(VkSemaphore image_ready_semaphore)
{
	if (this->invalid())
	{
		return std::numeric_limits<uint32_t>::max();
	}

	uint32_t image_index;
	VkResult result = vkAcquireNextImageKHR(this->device->get(), this->current.swapchain, std::numeric_limits<uint64_t>::max(), image_ready_semaphore, VK_NULL_HANDLE, &image_index);

	if (result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		this->invalidateSwapchain();
		return std::numeric_limits<uint32_t>::max();
	}

	return image_index;
}

VulkanSwapchain::Swapchain VulkanSwapchain::createSwapchain(VkPhysicalDevice physical_device, VkExtent2D surface_size, VkSurfaceKHR surface, VkSwapchainKHR old_swapchain)
{
	Swapchain new_swapchain = {};

	VkSurfaceCapabilitiesKHR capabilities;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, surface, &capabilities);

	//Create Swapchain
	VkSwapchainCreateInfoKHR create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	create_info.surface = surface;


	//Image Count
	new_swapchain.image_count = capabilities.minImageCount + 1;
	if (capabilities.maxImageCount > 0 && new_swapchain.image_count > capabilities.maxImageCount)
	{
		new_swapchain.image_count = capabilities.maxImageCount;
	}
	create_info.minImageCount = new_swapchain.image_count;


	//Format
	VkSurfaceFormatKHR format;
	{
		/*vector<VkSurfaceFormatKHR> formats;
		uint32_t format_count = 0;
		vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &format_count, nullptr);
		if (format_count != 0)
		{
			formats.resize(format_count);
			vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &format_count, formats.data());
		}*/
		//Force this format
		//TODO allow others
		format = { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
	}
	create_info.imageFormat = format.format;
	create_info.imageColorSpace = format.colorSpace;


	//Surface Size
	new_swapchain.size = chooseSwapExtent(capabilities, surface_size);
	create_info.imageExtent = new_swapchain.size;


	create_info.imageArrayLayers = 1;
	create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;


	//Present Mode
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
		create_info.presentMode = best_mode;
	}

	//QueueFamily
	{
		uint32_t queueFamilyIndices[] = { this->device->getGraphicsFamilyIndex(),  this->device->getPresentFamilyIndex() };

		if (queueFamilyIndices[0] != queueFamilyIndices[1])
		{
			create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			create_info.queueFamilyIndexCount = 2;
			create_info.pQueueFamilyIndices = queueFamilyIndices;
		}
		else
		{
			create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			create_info.queueFamilyIndexCount = 0;
			create_info.pQueueFamilyIndices = nullptr;
		}
	}


	create_info.preTransform = capabilities.currentTransform;
	create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	create_info.clipped = VK_TRUE;
	create_info.oldSwapchain = old_swapchain;

	VkResult result = vkCreateSwapchainKHR(this->device->get(), &create_info, nullptr, &new_swapchain.swapchain);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create swap chain!");
	}


	//Render Pass
	{
		VkAttachmentDescription colorAttachment = {};
		colorAttachment.format = format.format;
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference colorAttachmentRef = {};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;
		subpass.pDepthStencilAttachment = nullptr;

		VkSubpassDependency dependency = {};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		vector<VkAttachmentDescription> attachments = { colorAttachment };
		VkRenderPassCreateInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		renderPassInfo.pAttachments = attachments.data();
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;
		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &dependency;

		if (vkCreateRenderPass(this->device->get(), &renderPassInfo, nullptr, &new_swapchain.render_pass) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create screen render pass!");
		}
	}


	//Swapchain Images
	vkGetSwapchainImagesKHR(this->device->get(), new_swapchain.swapchain, &new_swapchain.image_count, nullptr);
	new_swapchain.images.resize(new_swapchain.image_count);
	vkGetSwapchainImagesKHR(device->get(), new_swapchain.swapchain, &new_swapchain.image_count, new_swapchain.images.data());


	//Swapchain Image Views
	new_swapchain.image_views.resize(new_swapchain.image_count);
	for (size_t i = 0; i < new_swapchain.image_views.size(); i++)
	{
		VkImageViewCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		create_info.image = new_swapchain.images[i];
		create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
		create_info.format = format.format;
		create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		create_info.subresourceRange.baseMipLevel = 0;
		create_info.subresourceRange.levelCount = 1;
		create_info.subresourceRange.baseArrayLayer = 0;
		create_info.subresourceRange.layerCount = 1;

		if (vkCreateImageView(device->get(), &create_info, nullptr, &new_swapchain.image_views[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create image views!");
		}
	}


	//Swapchain Framebuffers
	new_swapchain.framebuffers.resize(new_swapchain.image_count);
	for (size_t i = 0; i < new_swapchain.framebuffers.size(); i++)
	{
		List<VkImageView> attachments(1);
		attachments[0] = new_swapchain.image_views[i];

		VkFramebufferCreateInfo framebuffer_info = {};
		framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebuffer_info.renderPass = new_swapchain.render_pass;
		framebuffer_info.attachmentCount = (uint32_t)attachments.size();
		framebuffer_info.pAttachments = attachments.data();
		framebuffer_info.width = new_swapchain.size.width;
		framebuffer_info.height = new_swapchain.size.height;
		framebuffer_info.layers = 1;

		if (vkCreateFramebuffer(device->get(), &framebuffer_info, nullptr, &new_swapchain.framebuffers[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create frameBuffer!");
		}
	}

	return new_swapchain;
}

void VulkanSwapchain::destroySwapchain(Swapchain& swapchain)
{
	this->device->waitIdle();

	for (size_t i = 0; i < swapchain.image_count; i++)
	{
		vkDestroyFramebuffer(this->device->get(), swapchain.framebuffers[i], nullptr);
		vkDestroyImageView(this->device->get(), swapchain.image_views[i], nullptr);
	}

	vkDestroyRenderPass(this->device->get(), swapchain.render_pass, nullptr);
	vkDestroySwapchainKHR(this->device->get(), swapchain.swapchain, nullptr);
}
