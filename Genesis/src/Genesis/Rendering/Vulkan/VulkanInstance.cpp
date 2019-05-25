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
	this->swapchain = new VulkanSwapchain(this->device, window, this->surface);
	this->command_pool = new VulkanCommandPool(this->device, number_of_threads);

	//Allocator
	VmaAllocatorCreateInfo allocatorInfo = {};
	allocatorInfo.physicalDevice = this->device->getPhysicalDevice();
	allocatorInfo.device = this->device->getDevice();
	vmaCreateAllocator(&allocatorInfo, &this->allocator);

	//TEMP-ISH
	VkSemaphoreCreateInfo semaphoreInfo = {};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	VkFenceCreateInfo fenceInfo = {};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
	if (vkCreateSemaphore(this->device->getDevice(), &semaphoreInfo, nullptr, &this->image_available_semaphore) != VK_SUCCESS ||
		vkCreateSemaphore(this->device->getDevice(), &semaphoreInfo, nullptr, &this->render_finished_semaphore) != VK_SUCCESS ||
		vkCreateFence(this->device->getDevice(), &fenceInfo, nullptr, &this->in_flight_fence) != VK_SUCCESS) {

		throw std::runtime_error("failed to create semaphores!");
	}

	//TEMP
	this->create_TEMP();

	//Framebuffers
	this->swapchain_framebuffers = new VulkanSwapchainFramebuffers(this->device, this->swapchain, this->allocator, this->screen_render_pass);
}

VulkanInstance::~VulkanInstance()
{
	vkDeviceWaitIdle(this->device->getDevice());


	if (this->swapchain_framebuffers != nullptr)
	{
		delete this->swapchain_framebuffers;
	}

	this->delete_TEMP();

	vkDestroySemaphore(this->device->getDevice(), this->render_finished_semaphore, nullptr);
	vkDestroySemaphore(this->device->getDevice(), this->image_available_semaphore, nullptr);
	vkDestroyFence(this->device->getDevice(), this->in_flight_fence, nullptr);

	vmaDestroyAllocator(this->allocator);

	if (this->command_pool != nullptr)
	{
		delete this->command_pool;
	}

	if (this->swapchain != nullptr)
	{
		delete this->swapchain;
	}

	if (this->device != nullptr)
	{
		delete this->device;
	}

	this->delete_surface();

	if (this->debug_layer != nullptr)
	{
		delete this->debug_layer;
	}

	this->delete_instance();
}

bool Genesis::VulkanInstance::AcquireSwapchainImage(uint32_t& image_index)
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
		this->create_TEMP();
		this->swapchain_framebuffers = new VulkanSwapchainFramebuffers(this->device, this->swapchain, this->allocator, this->screen_render_pass);
	}

	VkResult result = vkAcquireNextImageKHR(this->device->getDevice(), this->swapchain->getSwapchain(), std::numeric_limits<uint64_t>::max(), this->image_available_semaphore, VK_NULL_HANDLE, &image_index);

	if (result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		//WAITS until work is done
		image_index = UINT32_MAX;
		vkDeviceWaitIdle(this->device->getDevice());

		//Delete old stuff
		if (this->swapchain_framebuffers != nullptr)
		{
			delete this->swapchain_framebuffers;
			this->swapchain_framebuffers = nullptr;
		}
		this->delete_TEMP();
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
		this->create_TEMP();
		this->swapchain_framebuffers = new VulkanSwapchainFramebuffers(this->device, this->swapchain, this->allocator, this->screen_render_pass);

		VkResult result = vkAcquireNextImageKHR(this->device->getDevice(), this->swapchain->getSwapchain(), std::numeric_limits<uint64_t>::max(), this->image_available_semaphore, VK_NULL_HANDLE, &image_index);
	}

	vkWaitForFences(this->device->getDevice(), 1, &this->in_flight_fence, VK_TRUE, std::numeric_limits<uint64_t>::max());
	vkResetFences(this->device->getDevice(), 1, &this->in_flight_fence);

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

void VulkanInstance::create_TEMP()
{
	VulkanShader shader = VulkanShader(this->device->getDevice(), "resources/Shaders/Vulkan/shader.vert.spv", "resources/Shaders/Vulkan/shader.frag.spv");

	if (true)
	{
		VkAttachmentDescription colorAttachment = {};
		colorAttachment.format = this->swapchain->getSwapchainFormat();
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentDescription depthAttachment = {};
		depthAttachment.format = VK_FORMAT_D32_SFLOAT;//HARDCODED for now findDepthFormat(this->context.device_properties.physical_device);
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

		if (vkCreateRenderPass(this->device->getDevice(), &renderPassInfo, nullptr, &this->screen_render_pass) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create render pass!");
		}
	}

	if (true)
	{
		VkPushConstantRange pushConstantRange = {};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		pushConstantRange.size = sizeof(matrix4F);
		pushConstantRange.offset = 0;

		VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pSetLayouts = nullptr;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
		pipelineLayoutInfo.pushConstantRangeCount = 1;

		if (vkCreatePipelineLayout(this->device->getDevice(), &pipelineLayoutInfo, nullptr, &this->colored_mesh_pipeline_layout) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create pipeline layout!");
		}
	}

	if (true)
	{
		VertexInput input;
		input.binding_descriptions = Vertex::getBindingDescriptions();
		input.attribute_descriptions = Vertex::getAttributeDescriptions();

		this->colored_mesh_screen_pipeline = new VulkanRenderPipline(this, this->colored_mesh_pipeline_layout, this->screen_render_pass, &shader, &input);
	}
}

void VulkanInstance::delete_TEMP()
{
	delete this->colored_mesh_screen_pipeline;

	vkDestroyPipelineLayout(this->device->getDevice(), this->colored_mesh_pipeline_layout, nullptr);
	vkDestroyRenderPass(this->device->getDevice(), this->screen_render_pass, nullptr);

}
