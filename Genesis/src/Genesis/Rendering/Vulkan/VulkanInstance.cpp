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

	this->frames_in_flight = Array<VulkanFrame>(NUM_OF_FRAMES);
	for (int i = 0; i < this->frames_in_flight.size(); i++)
	{
		VulkanFrame* frame = &this->frames_in_flight[i];
		frame->command_buffer = new VulkanMultithreadCommandBuffer(this->primary_command_pool, &this->secondary_command_pools);

		frame->image_available_semaphore = this->device->createSemaphore();
		frame->command_buffer_done_fence = this->device->createFence();
		frame->command_buffer_done_semaphore = this->device->createSemaphore();
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

	this->descriptor_pool = new VulkanDescriptorPool(this->device->get(), VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, this->pipeline_manager->textured_descriptor_layout, 2048);

	this->shadow_pass_layout = new VulkanFramebufferLayout(this->device->get(), Array<VkFormat>(), this->swapchain->getSwapchainDepthFormat());

}

VulkanInstance::~VulkanInstance()
{
	vkDestroySampler(this->device->get(), this->linear_sampler, nullptr);

	vkDeviceWaitIdle(this->device->get());

	delete this->shadow_pass_layout;

	delete this->descriptor_pool;

	delete this->pipeline_manager;

	if (this->swapchain_framebuffers != nullptr)
	{
		delete this->swapchain_framebuffers;
	}

	for (int i = 0; i < this->frames_in_flight.size(); i++)
	{
		VulkanFrame* frame = &this->frames_in_flight[i];
		delete frame->command_buffer;
		this->device->destroySemaphore( frame->image_available_semaphore);
		this->device->destroyFence(frame->command_buffer_done_fence);
		this->device->destroySemaphore(frame->command_buffer_done_semaphore);
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