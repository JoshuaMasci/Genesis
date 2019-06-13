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

	VkSemaphoreCreateInfo semaphoreInfo = {};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo = {};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	const uint32_t NUM_OF_FRAMES = 4;
	this->frames_in_flight = Array<VulkanFrame>(NUM_OF_FRAMES);
	for (int i = 0; i < this->frames_in_flight.size(); i++)
	{
		VulkanFrame* frame = &this->frames_in_flight[i];
		frame->command_buffer = new VulkanMultithreadCommandBuffer(this->device, this->command_pool->graphics_command_pool, number_of_threads);

		if (vkCreateSemaphore(this->device->getDevice(), &semaphoreInfo, nullptr, &frame->image_available_semaphore) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create sync objects!");
		}

		if (vkCreateFence(this->device->getDevice(), &fenceInfo, nullptr, &frame->command_buffer_done_fence) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create fence objects!");
		}

		if (vkCreateSemaphore(this->device->getDevice(), &semaphoreInfo, nullptr, &frame->command_buffer_done_semaphore) != VK_SUCCESS)
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

		if (vkCreateSampler(this->device->getDevice(), &samplerInfo, nullptr, &this->linear_sampler) != VK_SUCCESS) 
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

		if (vkCreateDescriptorPool(this->device->getDevice(), &pool_info, nullptr, &this->descriptor_pool) != VK_SUCCESS) 
		{
			throw std::runtime_error("failed to create descriptor pool!");
		}
	}
}

VulkanInstance::~VulkanInstance()
{
	vkDestroyDescriptorPool(this->device->getDevice(), this->descriptor_pool, nullptr);

	vkDestroySampler(this->device->getDevice(), this->linear_sampler, nullptr);

	vkDeviceWaitIdle(this->device->getDevice());

	if (this->pipeline_manager != nullptr)
	{
		delete this->pipeline_manager;
	}

	if (this->swapchain_framebuffers != nullptr)
	{
		delete this->swapchain_framebuffers;
	}

	//TEMP
	//this->delete_TEMP();

	vmaDestroyAllocator(this->allocator);

	for (int i = 0; i < this->frames_in_flight.size(); i++)
	{
		VulkanFrame* frame = &this->frames_in_flight[i];
		delete frame->command_buffer;
		vkDestroySemaphore(this->device->getDevice(), frame->image_available_semaphore, nullptr);
		vkDestroyFence(this->device->getDevice(), frame->command_buffer_done_fence, nullptr);
		vkDestroySemaphore(this->device->getDevice(), frame->command_buffer_done_semaphore, nullptr);
	}

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

	VkResult result = vkAcquireNextImageKHR(this->device->getDevice(), this->swapchain->getSwapchain(), std::numeric_limits<uint64_t>::max(), signal_semaphore, VK_NULL_HANDLE, &image_index);

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

		VkResult result = vkAcquireNextImageKHR(this->device->getDevice(), this->swapchain->getSwapchain(), std::numeric_limits<uint64_t>::max(), signal_semaphore, VK_NULL_HANDLE, &image_index);
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