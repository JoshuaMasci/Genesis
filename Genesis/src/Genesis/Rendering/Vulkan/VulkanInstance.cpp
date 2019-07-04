#include "VulkanInstance.hpp"

#include "Genesis/Rendering/Vulkan/VulkanPhysicalDevicePicker.hpp"
#include "Genesis/Rendering/Vulkan/VulkanShader.hpp"
#include "Genesis/Rendering/Vulkan/VulkanRenderPipline.hpp"

using namespace Genesis;

VulkanInstance::VulkanInstance(Window* window, uint32_t number_of_threads)
{
	this->window = window;

	this->create_instance("Sandbox", VK_MAKE_VERSION(0, 0, 1));
	if (this->use_debug_layers)
	{
		this->debug_layer = new VulkanDebugLayer(this->instance);
	}
	this->create_surface(window);

	this->device = new VulkanDevice(VulkanPhysicalDevicePicker::pickDevice(this->instance, this->surface), this);
	this->allocator = new VulkanAllocator(this->device);

	this->swapchain = new VulkanSwapchain(this->device, window, this->surface);

	this->primary_command_pool = new VulkanCommandPool(this->device->get(), this->device->getGraphicsFamilyIndex(), VK_COMMAND_BUFFER_LEVEL_PRIMARY, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
	this->secondary_command_pools = Array<VulkanCommandPool*>(number_of_threads);
	for (int i = 0; i < this->secondary_command_pools.size(); i++)
	{
		this->secondary_command_pools[i] = new VulkanCommandPool(this->device->get(), this->device->getGraphicsFamilyIndex(), VK_COMMAND_BUFFER_LEVEL_SECONDARY, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
	}

	VkSemaphoreCreateInfo semaphoreInfo = {};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo = {};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	const uint32_t NUM_OF_FRAMES = 3;
	this->frames_in_flight = Array<VulkanFrame>(NUM_OF_FRAMES);
	for (int i = 0; i < this->frames_in_flight.size(); i++)
	{
		VulkanFrame* frame = &this->frames_in_flight[i];
		frame->command_buffer = new VulkanMultithreadCommandBuffer(this->primary_command_pool, &this->secondary_command_pools);

		if (vkCreateSemaphore(this->device->get(), &semaphoreInfo, nullptr, &frame->image_available_semaphore) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create sync objects!");
		}

		if (vkCreateFence(this->device->get(), &fenceInfo, nullptr, &frame->command_buffer_done_fence) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create fence objects!");
		}

		if (vkCreateSemaphore(this->device->get(), &semaphoreInfo, nullptr, &frame->command_buffer_done_semaphore) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create sync objects!");
		}
	}

	this->pipeline_manager = new VulkanPiplineManager(this->device);
	this->pipeline_manager->buildScreenRenderPass(this->swapchain->getSwapchainFormat(), this->swapchain->getSwapchainDepthFormat());
	this->pipeline_manager->rebuildSwapchainPipelines(this->swapchain->getSwapchainExtent());

	//Framebuffers
	this->swapchain_framebuffers = new VulkanSwapchainFramebuffers(this->device, this->swapchain, this->allocator, this->pipeline_manager->getScreenRenderPass());

	{
		VkSamplerCreateInfo samplerInfo = {};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = VK_FILTER_LINEAR;
		samplerInfo.minFilter = VK_FILTER_LINEAR;
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.anisotropyEnable = VK_FALSE;
		samplerInfo.maxAnisotropy = 0;
		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerInfo.unnormalizedCoordinates = VK_FALSE;
		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

		if (vkCreateSampler(this->device->get(), &samplerInfo, nullptr, &this->linear_sampler) != VK_SUCCESS) 
		{
			throw std::runtime_error("failed to create texture sampler!");
		}
	}

	{
		Array<VkDescriptorPoolSize> pool_sizes(1);
		pool_sizes[0].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		pool_sizes[0].descriptorCount = this->swapchain->getSwapchainImageCount();

		VkDescriptorPoolCreateInfo pool_info = {};
		pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		pool_info.poolSizeCount = static_cast<uint32_t>(pool_sizes.size());
		pool_info.pPoolSizes = pool_sizes.data();
		pool_info.maxSets = this->swapchain->getSwapchainImageCount();

		if (vkCreateDescriptorPool(this->device->get(), &pool_info, nullptr, &this->descriptor_pool) != VK_SUCCESS) 
		{
			throw std::runtime_error("failed to create descriptor pool!");
		}
	}

	this->buildShadowRenderPass(this->swapchain->getSwapchainDepthFormat());
}

VulkanInstance::~VulkanInstance()
{
	vkDestroyPipelineLayout(this->device->get(), this->shadow_pipeline_layout, nullptr);
	vkDestroyRenderPass(this->device->get(), this->shadow_render_pass, nullptr);

	vkDestroyDescriptorPool(this->device->get(), this->descriptor_pool, nullptr);

	vkDestroySampler(this->device->get(), this->linear_sampler, nullptr);

	vkDeviceWaitIdle(this->device->get());

	if (this->pipeline_manager != nullptr)
	{
		delete this->pipeline_manager;
	}

	if (this->swapchain_framebuffers != nullptr)
	{
		delete this->swapchain_framebuffers;
	}

	for (int i = 0; i < this->frames_in_flight.size(); i++)
	{
		VulkanFrame* frame = &this->frames_in_flight[i];
		delete frame->command_buffer;
		vkDestroySemaphore(this->device->get(), frame->image_available_semaphore, nullptr);
		vkDestroyFence(this->device->get(), frame->command_buffer_done_fence, nullptr);
		vkDestroySemaphore(this->device->get(), frame->command_buffer_done_semaphore, nullptr);
	}

	delete this->primary_command_pool;
	for (int i = 0; i < this->secondary_command_pools.size(); i++)
	{
		delete this->secondary_command_pools[i];
	}

	if (this->swapchain != nullptr)
	{
		delete this->swapchain;
	}

	delete this->allocator;

	delete this->device;

	this->delete_surface();

	if (this->debug_layer != nullptr)
	{
		delete this->debug_layer;
	}

	this->delete_instance();
}

bool VulkanInstance::AcquireSwapchainImage(uint32_t& image_index, VkSemaphore signal_semaphore)
{
	if (this->swapchain == nullptr)
	{
		//Checks if the swapchain can be created
		VkSurfaceCapabilitiesKHR capabilities;
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(this->device->getPhysicalDevice(), this->surface, &capabilities);
		if (capabilities.maxImageExtent.width <= 1, capabilities.maxImageExtent.height <= 1)
		{
			return false;
		}

		//Creates swapchain
		this->swapchain = new VulkanSwapchain(this->device, this->window, this->surface);
		this->swapchain_framebuffers = new VulkanSwapchainFramebuffers(this->device, this->swapchain, this->allocator, this->pipeline_manager->getScreenRenderPass());
		this->pipeline_manager->rebuildSwapchainPipelines(this->swapchain->getSwapchainExtent());
	}

	VkResult result = vkAcquireNextImageKHR(this->device->get(), this->swapchain->get(), std::numeric_limits<uint64_t>::max(), signal_semaphore, VK_NULL_HANDLE, &image_index);

	if (result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		//WAITS until work is done
		image_index = UINT32_MAX;
		vkDeviceWaitIdle(this->device->get());

		//Delete old stuff
		if (this->swapchain_framebuffers != nullptr)
		{
			delete this->swapchain_framebuffers;
			this->swapchain_framebuffers = nullptr;
		}
		//this->delete_TEMP();
		if (this->swapchain != nullptr)
		{
			delete this->swapchain;
			this->swapchain = nullptr;
		}

		//Checks if the swapchain can be created
		VkSurfaceCapabilitiesKHR capabilities;
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(this->device->getPhysicalDevice(), this->surface, &capabilities);
		if (capabilities.maxImageExtent.width <= 1, capabilities.maxImageExtent.height <= 1)
		{
			return false;
		}

		//Creates swapchain
		this->swapchain = new VulkanSwapchain(this->device, this->window, this->surface);
		this->swapchain_framebuffers = new VulkanSwapchainFramebuffers(this->device, this->swapchain, this->allocator, this->pipeline_manager->getScreenRenderPass());
		this->pipeline_manager->rebuildSwapchainPipelines(this->swapchain->getSwapchainExtent());

		VkResult result = vkAcquireNextImageKHR(this->device->get(), this->swapchain->get(), std::numeric_limits<uint64_t>::max(), signal_semaphore, VK_NULL_HANDLE, &image_index);
	}

	return true;
}

vector<const char*> VulkanInstance::getExtensions()
{
	vector<const char*> extensions;

	extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);

#ifdef VK_USE_PLATFORM_WIN32_KHR
	extensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#endif

	if (this->use_debug_layers)
	{
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
	}

	return extensions;
}

vector<const char*> VulkanInstance::getDeviceExtensions()
{
	vector<const char*> extensions;

	extensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

	return extensions;
}

vector<const char*> VulkanInstance::getLayers()
{
	vector<const char*> layers;

	if (this->use_debug_layers)
	{
		layers.push_back("VK_LAYER_LUNARG_standard_validation");
	}

	return layers;
}

void VulkanInstance::buildShadowRenderPass(VkFormat depth_format)
{
	VkAttachmentDescription attachment_description = {};
	attachment_description.format = depth_format;
	attachment_description.samples = VK_SAMPLE_COUNT_1_BIT;
	attachment_description.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR; //Clear depth at beginning of the render pass
	attachment_description.storeOp = VK_ATTACHMENT_STORE_OP_STORE; //We will read from depth, so it's important to store the depth attachment results
	attachment_description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachment_description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachment_description.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED; //We don't care about initial layout of the attachment
	attachment_description.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL; //Attachment will be transitioned to shader read at render pass end

	VkAttachmentReference depthReference = {};
	depthReference.attachment = 0;
	depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL; //Attachment will be used as depth/stencil during render pass

	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 0; //No color attachments
	subpass.pDepthStencilAttachment = &depthReference; //Reference to our depth attachment

	//Use subpass dependencies for layout transitions
	Array<VkSubpassDependency> dependencies(2);

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

	VkRenderPassCreateInfo render_pass_info = {};
	render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;

	render_pass_info.attachmentCount = 1;
	render_pass_info.pAttachments = &attachment_description;
	render_pass_info.subpassCount = 1;
	render_pass_info.pSubpasses = &subpass;
	render_pass_info.dependencyCount = (uint32_t) dependencies.size();
	render_pass_info.pDependencies = dependencies.data();

	if (vkCreateRenderPass(this->device->get(), &render_pass_info, nullptr, &this->shadow_render_pass) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create render pass!");
	}


	//BUILD PIPELINE LAYOUT AS WELL
	VkPushConstantRange pushConstantRange = {};
	pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	pushConstantRange.size = sizeof(matrix4F);
	pushConstantRange.offset = 0;

	VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

	pipelineLayoutInfo.setLayoutCount = 0;

	pipelineLayoutInfo.pushConstantRangeCount = 1;
	pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

	if (vkCreatePipelineLayout(this->device->get(), &pipelineLayoutInfo, nullptr, &this->shadow_pipeline_layout) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create pipeline layout!");
	}
}

void VulkanInstance::create_instance(const char* app_name, uint32_t app_version)
{
	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = app_name;
	appInfo.applicationVersion = app_version;
	appInfo.pEngineName = "Genesis";
	appInfo.engineVersion = VK_MAKE_VERSION(0, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_1;

	VkInstanceCreateInfo create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	create_info.pApplicationInfo = &appInfo;

	vector<const char*> extensions = this->getExtensions();
	if (extensions.size() > 0)
	{
		create_info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		create_info.ppEnabledExtensionNames = extensions.data();
	}
	else
	{
		create_info.enabledExtensionCount = 0;
	}

	vector<const char*> layers = this->getLayers();
	if (layers.size() > 0)
	{
		create_info.enabledLayerCount = static_cast<uint32_t>(layers.size());
		create_info.ppEnabledLayerNames = layers.data();
	}
	else
	{
		create_info.enabledLayerCount = 0;
	}

	VkResult result = vkCreateInstance(&create_info, nullptr, &this->instance);

	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create Vulkan Instance!");
	}
}

void VulkanInstance::delete_instance()
{
	vkDestroyInstance(this->instance, nullptr);
}

void VulkanInstance::create_surface(Window* window)
{
	VkResult result = (VkResult)1;//Not Success

#ifdef VK_USE_PLATFORM_WIN32_KHR
	VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = { VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR };

	surfaceCreateInfo.hinstance = GetModuleHandle(0);
	surfaceCreateInfo.hwnd = (HWND)window->getNativeWindowHandle();

	result = vkCreateWin32SurfaceKHR(this->instance, &surfaceCreateInfo, NULL, &this->surface);
#endif

	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("failed to attach to surface");
	}
}

void VulkanInstance::delete_surface()
{
	vkDestroySurfaceKHR(this->instance, this->surface, nullptr);
}