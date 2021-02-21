#include "VulkanRenderer.hpp"

#include <stdio.h>

#include "Image.hpp"

namespace VulkanRenderer
{
	static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity, VkDebugUtilsMessageTypeFlagsEXT message_type, const VkDebugUtilsMessengerCallbackDataEXT* callback_data, void* user_data)
	{
		printf("Vulkan Validation Layer: %s\n", callback_data->pMessage);
		return VK_FALSE;
	}

	void DebugLayer::create(VkInstance instance)
	{
		this->instance = instance;

		VkDebugUtilsMessengerCreateInfoEXT create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		create_info.pfnUserCallback = debug_callback;
		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(this->instance, "vkCreateDebugUtilsMessengerEXT");
		if (func)
		{
			VK_ASSERT(func(this->instance, &create_info, nullptr, &this->debug_messenger));
		}
	}

	void DebugLayer::destroy()
	{
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(this->instance, "vkDestroyDebugUtilsMessengerEXT");
		if (func)
		{
			func(this->instance, this->debug_messenger, nullptr);
		}
	}

	VkInstance create_instance(VulkanApplication& app, VulkanSettings& settings, std::vector<const char*>& extensions, std::vector<const char*>& layers)
	{
		VkApplicationInfo appInfo = {};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = app.app_name;
		appInfo.applicationVersion = app.app_version;
		appInfo.pEngineName = "VulkanRenderer";
		appInfo.engineVersion = VK_MAKE_VERSION(0, 1, 0);
		appInfo.apiVersion = 0;

		VkInstanceCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		create_info.pApplicationInfo = &appInfo;
		create_info.enabledExtensionCount = 0;
		create_info.enabledLayerCount = 0;
		create_info.enabledExtensionCount = (uint32_t)extensions.size();
		create_info.ppEnabledExtensionNames = extensions.data();
		create_info.enabledLayerCount = (uint32_t)layers.size();
		create_info.ppEnabledLayerNames = layers.data();

		VkInstance instance;
		VK_ASSERT(vkCreateInstance(&create_info, nullptr, &instance));
		return instance;
	}

	VkSurfaceKHR create_surface(VkInstance instance, void* window_handle)
	{
		VkSurfaceKHR surface = VK_NULL_HANDLE;

#ifdef WIN32
		VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = { VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR };
		surfaceCreateInfo.hinstance = GetModuleHandle(0);
		surfaceCreateInfo.hwnd = (HWND)window_handle;
		VK_ASSERT(vkCreateWin32SurfaceKHR(instance, &surfaceCreateInfo, NULL, &surface));
#endif

		return surface;
	}

	VkPhysicalDevice get_physical_device(VkInstance instance, VkSurfaceKHR surface)
	{
		uint32_t device_count = 0;
		vkEnumeratePhysicalDevices(instance, &device_count, nullptr);
		std::vector<VkPhysicalDevice> devices(device_count);
		vkEnumeratePhysicalDevices(instance, &device_count, devices.data());

		for (VkPhysicalDevice device : devices)
		{
			VkBool32 supported = VK_FALSE;
			VK_ASSERT(vkGetPhysicalDeviceSurfaceSupportKHR(device, 0, surface, &supported));

			if (supported == VK_TRUE)
			{
				return device;
			}
		}

		return VK_NULL_HANDLE;//Just choose the first device for now
	}

	VulkanRenderer::VulkanRenderer(VulkanApplication& app, VulkanSettings& settings, VulkanWindow& window)
	{
		std::vector<const char*> extensions;
		extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
		//extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
#ifdef VK_USE_PLATFORM_WIN32_KHR
		extensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#endif

		std::vector<const char*> device_extensions;
		device_extensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

		std::vector<const char*> layers;
		layers.push_back("VK_LAYER_KHRONOS_validation");

		this->instance = create_instance(app, settings, extensions, layers);
		this->debug_layer.create(this->instance);		
		this->surface = create_surface(this->instance, window.handle);

		this->device = new Device(this->instance, get_physical_device(this->instance, this->surface), device_extensions, layers);
		this->swapchain = new Swapchain(this->device, this->surface, { window.width, window.height });

		this->wait_fence = this->device->create_fence();
		this->image_wait_semaphore = this->device->create_semaphore();
		this->present_wait_semaphore = this->device->create_semaphore();

		this->primary_command_pool = new CommandPool(this->device->get(), this->device->get_queue_index(), VK_COMMAND_BUFFER_LEVEL_PRIMARY, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
	}

	VulkanRenderer::~VulkanRenderer()
	{
		this->device->wait_idle();

		delete this->primary_command_pool;

		this->device->destroy_semaphore(this->image_wait_semaphore);
		this->device->destroy_semaphore(this->present_wait_semaphore);
		this->device->destroy_fence(this->wait_fence);

		delete this->swapchain;
		delete this->device;

		vkDestroySurfaceKHR(this->instance, this->surface, nullptr);
		this->debug_layer.destroy();
		vkDestroyInstance(this->instance, nullptr);
	}

	/*Image* create_image(Device* device, const AttachmentCreateInfo& attachment_create_info, VkExtent2D size)
	{
		VkImageCreateInfo image_info = {};
		image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		image_info.imageType = VK_IMAGE_TYPE_2D;
		image_info.extent.width = size.width;
		image_info.extent.height = size.height;
		image_info.extent.depth = 1;
		image_info.mipLevels = 1;
		image_info.arrayLayers = 1;
		image_info.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		image_info.format = attachment_create_info.format;
		image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
		image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		image_info.samples = (VkSampleCountFlagBits)attachment_create_info.samples;
		image_info.flags = 0;

		return new Image(device, image_info, VMA_MEMORY_USAGE_GPU_ONLY);
	};

	VkRenderPass create_render_pass(Device* device, const RenderPassDescription* render_pass_info)
	{
		/*std::vector<VkAttachmentDescription> attachment_descriptions;
		std::vector<VkAttachmentReference> color_attachment_references;

		for (auto color_attachment : render_pass_info.)
		{
			VkAttachmentDescription color_description = {};
			color_description.format = color_attachment.format;
			color_description.samples = (VkSampleCountFlagBits)color_attachment.samples;
			color_description.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			color_description.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			color_description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			color_description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			color_description.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			color_description.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

			VkAttachmentReference color_reference = {};
			color_reference.attachment = (uint32_t)attachment_descriptions.size();
			color_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

			attachment_descriptions.push_back(color_description);
			color_attachment_references.push_back(color_reference);
		}

		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = (uint32_t)color_attachment_references.size();
		subpass.pColorAttachments = color_attachment_references.data();

		if (render_pass_info.depth_attachment != nullptr)
		{
			AttachmentCreateInfo* depth_info = render_pass_info.depth_attachment;
			uint32_t depth_position = (uint32_t)attachment_descriptions.size();

			VkAttachmentReference depth_reference = {};
			depth_reference.attachment = depth_position;
			depth_reference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			subpass.pDepthStencilAttachment = &depth_reference;

			VkAttachmentDescription depth_description = {};
			depth_description.format = depth_info->format;
			depth_description.samples = (VkSampleCountFlagBits)depth_info->samples;
			depth_description.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			depth_description.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			depth_description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			depth_description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
			depth_description.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			depth_description.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

			attachment_descriptions.push_back(depth_description);
		}
		else
		{
			subpass.pDepthStencilAttachment = VK_NULL_HANDLE;
		}

		VkSubpassDependency dependencies[2];
		dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
		dependencies[0].dstSubpass = 0;
		dependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
		dependencies[0].dstStageMask = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
		dependencies[0].srcAccessMask = 0;
		dependencies[0].dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

		dependencies[1].srcSubpass = 0;
		dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
		dependencies[1].srcStageMask = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
		dependencies[1].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		dependencies[1].srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		dependencies[1].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

		VkRenderPassCreateInfo vk_render_pass_info = {};
		vk_render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		vk_render_pass_info.attachmentCount = (uint32_t)attachment_descriptions.size();
		vk_render_pass_info.pAttachments = attachment_descriptions.data();
		vk_render_pass_info.subpassCount = 1;
		vk_render_pass_info.pSubpasses = &subpass;
		vk_render_pass_info.dependencyCount = 2;
		vk_render_pass_info.pDependencies = dependencies;

		VkRenderPass render_pass = VK_NULL_HANDLE;
		//VK_ASSERT(vkCreateRenderPass(device->get(), &vk_render_pass_info, nullptr, &render_pass));
		return render_pass;
	}

	struct RenderPassHandle
	{
		VkExtent2D size = {};
		std::vector<Image*> color_attachments;
		Image* depth_attachment = nullptr;

		VkRenderPass render_pass = VK_NULL_HANDLE;
		VkFramebuffer framebuffer = VK_NULL_HANDLE;
	};*/

	void VulkanRenderer::render(FrameGraph* frame_graph)
	{
		/*vkWaitForFences(this->device->get(), 1, &this->wait_fence, VK_TRUE, std::numeric_limits<uint64_t>::max());
		vkResetFences(this->device->get(), 1, &this->wait_fence);
		uint32_t image_index = this->swapchain->get_next_image(this->image_wait_semaphore);

		std::unordered_map<AttachmentId, Image*> attachments;

		for (auto& render_pass_info: frame_graph->render_passes)
		{
			RenderPassHandle render_pass_handle = {};

			VkExtent2D render_pass_size = { 1, 1 };
			if (render_pass_info->size_class == AttachmentSizeClass::SwapchainRelative)
			{
				VkExtent2D swapchain_size = this->swapchain->get_image_extent();
				render_pass_size.width = (uint32_t)(render_pass_info->size[0] * swapchain_size.width);
				render_pass_size.height = (uint32_t)(render_pass_info->size[1] * swapchain_size.height);
			}
			else if (render_pass_info->size_class == AttachmentSizeClass::Absolute)
			{
				render_pass_size.width = (uint32_t)render_pass_info->size[0];
				render_pass_size.height = (uint32_t)render_pass_info->size[1];
			}

			render_pass_handle.color_attachments.reserve(render_pass_info->output_color_attachments.size());
			for (size_t i = 0; i < render_pass_handle.color_attachments.size(); i++)
			{
				Image* attachment = create_image(this->device, render_pass_info->output_color_attachments[i].create_info, render_pass_size);
				render_pass_handle.color_attachments.push_back(attachment);
				attachments[render_pass_info->output_color_attachments[i].id] = attachment;
			}

			render_pass_handle.render_pass = create_render_pass(this->device, render_pass_info);
		}
		
		VkCommandBuffer command_buffer = this->primary_command_pool->get_command_buffer();
		VkCommandBufferBeginInfo begin_info = {};
		begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		vkBeginCommandBuffer(command_buffer, &begin_info);
		VkImage image = this->swapchain->get_image(image_index);
		VkImageMemoryBarrier barrier = {};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		barrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = image;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = 0;
		VkPipelineStageFlags source_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		VkPipelineStageFlags destination_stage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
		vkCmdPipelineBarrier(
			command_buffer,
			source_stage, destination_stage,
			0,
			0, nullptr,
			0, nullptr,
			1, &barrier
		);
		vkEndCommandBuffer(command_buffer);

		{
			VkSemaphore wait_semaphore = this->image_wait_semaphore;
			VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;

			VkSubmitInfo graphics_submit_info = {};
			graphics_submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			graphics_submit_info.waitSemaphoreCount = 1;
			graphics_submit_info.pWaitSemaphores = &wait_semaphore;
			graphics_submit_info.pWaitDstStageMask = &wait_stage;

			graphics_submit_info.signalSemaphoreCount = 1;
			graphics_submit_info.pSignalSemaphores = &this->present_wait_semaphore;

			graphics_submit_info.commandBufferCount = 1;
			graphics_submit_info.pCommandBuffers = &command_buffer;

			//Submit command buffers
			VK_ASSERT(vkQueueSubmit(this->device->get_queue(), 1, &graphics_submit_info, this->wait_fence));
		}

		this->swapchain->present_image(image_index, this->present_wait_semaphore);*/
	}
}