#include "VulkanInstance.hpp"

#include "Genesis/Rendering/Vulkan/VulkanPhysicalDevicePicker.hpp"
#include "Genesis/Rendering/Vulkan/VulkanShader.hpp"
#include "Genesis/Rendering/Vulkan/VulkanPipline.hpp"

using namespace Genesis;

VkSurfaceKHR create_surface(VkInstance instance, Window* window)
{
	VkResult result = (VkResult)1;//Not Success
	VkSurfaceKHR surface = VK_NULL_HANDLE;

#ifdef VK_USE_PLATFORM_WIN32_KHR
	VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = { VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR };

	surfaceCreateInfo.hinstance = GetModuleHandle(0);
	surfaceCreateInfo.hwnd = (HWND)window->getNativeWindowHandle();

	result = vkCreateWin32SurfaceKHR(instance, &surfaceCreateInfo, NULL, &surface);
#endif

	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("failed to attach to surface");
	}

	return surface;
}

VulkanInstance::VulkanInstance(Window* window, uint32_t thread_count, uint32_t FRAME_COUNT)
	:FRAME_COUNT(FRAME_COUNT)
{
	this->window = window;

	this->create_instance("Sandbox", VK_MAKE_VERSION(0, 0, 1));
	if (this->use_debug_layers)
	{
		this->debug_layer = new VulkanDebugLayer(this->instance);
	}

	this->surface = create_surface(this->instance, this->window);

	vector<const char*> device_extensions = this->getDeviceExtensions();
	vector<const char*> layers = this->getLayers();
	this->device = new VulkanDevice(VulkanPhysicalDevicePicker::pickDevice(this->instance, surface), surface, device_extensions, layers);

	this->allocator = new VulkanAllocator(this->device);

	vector2U window_size = window->getWindowSize();
	this->swapchain = new VulkanSwapchain(this->device, {window_size.x, window_size.y}, this->surface);

	this->primary_graphics_pool = new VulkanCommandPool(this->device->get(), this->device->getGraphicsFamilyIndex(), VK_COMMAND_BUFFER_LEVEL_PRIMARY, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

	this->threads.resize(thread_count);
	for (size_t i = 0; i < thread_count; i++)
	{
		this->threads[i].secondary_graphics_pool = new VulkanCommandPool(this->device->get(), this->device->getGraphicsFamilyIndex(), VK_COMMAND_BUFFER_LEVEL_SECONDARY, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

		this->threads[i].descriptor_pool = new VulkanDescriptorPool(this->device->get(), this->FRAME_COUNT, 800000,
			{
				{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 8000},
				{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 8000}
			});
	}

	this->uniform_pool = new VulkanUniformPool(this->allocator, this->FRAME_COUNT);

	this->pipeline_pool = new VulkanPipelinePool(this->device->get(), thread_count);
	this->render_pass_manager = new VulkanRenderPassPool(this->device->get());

	{
		VkSamplerCreateInfo sampler_info = {};
		sampler_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		sampler_info.magFilter = VK_FILTER_NEAREST;
		sampler_info.minFilter = VK_FILTER_LINEAR;
		sampler_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		sampler_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		sampler_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		sampler_info.anisotropyEnable = VK_FALSE;
		sampler_info.maxAnisotropy = 0;
		sampler_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		sampler_info.unnormalizedCoordinates = VK_FALSE;
		sampler_info.compareEnable = VK_FALSE;
		sampler_info.compareOp = VK_COMPARE_OP_ALWAYS;
		sampler_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

		if (vkCreateSampler(this->device->get(), &sampler_info, nullptr, &this->texture_sampler) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create texture sampler!");
		}
	}

	{
		VkSamplerCreateInfo sampler_info = {};
		sampler_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		sampler_info.magFilter = VK_FILTER_NEAREST;
		sampler_info.minFilter = VK_FILTER_NEAREST;
		sampler_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
		sampler_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
		sampler_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
		sampler_info.anisotropyEnable = VK_FALSE;
		sampler_info.maxAnisotropy = 0;
		sampler_info.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
		sampler_info.unnormalizedCoordinates = VK_FALSE;
		sampler_info.compareEnable = VK_FALSE;
		sampler_info.compareOp = VK_COMPARE_OP_ALWAYS;
		sampler_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

		if (vkCreateSampler(this->device->get(), &sampler_info, nullptr, &this->view_sampler) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create texture sampler!");
		}
	}

	this->frames_in_flight = Array<VulkanFrame>(this->FRAME_COUNT);
	for (uint32_t i = 0; i < this->frames_in_flight.size(); i++)
	{
		VulkanFrame* frame = &this->frames_in_flight[i];
		frame->command_buffer = new VulkanCommandBuffer(0, i, this->device->get(), this->primary_graphics_pool, this->pipeline_pool, this->threads[0].descriptor_pool, this->uniform_pool, this->texture_sampler);
		frame->image_available_semaphore = this->device->createSemaphore();
		frame->command_buffer_done_fence = this->device->createFence();
		frame->command_buffer_done_semaphore = this->device->createSemaphore();
	}

	this->descriptor_pool = new VulkanDescriptorPool(this->device->get(), this->FRAME_COUNT, 800000,
		{
			{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 8000},
			{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 8000}
		});

	//Resource Deleters
	//Add 1 to the delete delay just to make sure it's totaly out of the pipeline
	uint8_t delay_cycles = (uint8_t) this->FRAME_COUNT + 1;
	this->buffer_deleter = new DelayedResourceDeleter<VulkanBuffer>(delay_cycles);
	this->texture_deleter = new DelayedResourceDeleter<VulkanTexture>(delay_cycles);
	this->view_deleter = new DelayedResourceDeleter<VulkanView>(delay_cycles);
	this->shader_deleter = new DelayedResourceDeleter<VulkanShader>(delay_cycles);
}

VulkanInstance::~VulkanInstance()
{
	//Resource Deleters
	delete this->buffer_deleter;
	delete this->texture_deleter;
	delete this->view_deleter;
	delete this->shader_deleter;

	vkDeviceWaitIdle(this->device->get());

	vkDestroySampler(this->device->get(), this->view_sampler, nullptr);
	vkDestroySampler(this->device->get(), this->texture_sampler, nullptr);

	delete this->descriptor_pool;
	delete this->uniform_pool;
	
	for (size_t i = 0; i < this->threads.size(); i++)
	{
		delete this->threads[i].descriptor_pool;
		delete this->threads[i].secondary_graphics_pool;
	}

	delete this->pipeline_pool;
	delete this->render_pass_manager;

	for (size_t i = 0; i < this->frames_in_flight.size(); i++)
	{
		VulkanFrame* frame = &this->frames_in_flight[i];
		delete frame->command_buffer;
		this->device->destroySemaphore( frame->image_available_semaphore);
		this->device->destroyFence(frame->command_buffer_done_fence);
		this->device->destroySemaphore(frame->command_buffer_done_semaphore);
	}

	delete this->primary_graphics_pool;

	delete this->swapchain;

	delete this->allocator;

	delete this->device;

	vkDestroySurfaceKHR(this->instance, this->surface, nullptr);

	if (this->debug_layer != nullptr)
	{
		delete this->debug_layer;
	}

	this->delete_instance();
}

void VulkanInstance::acquireSwapchainImage(uint32_t& image_index, VkSemaphore signal_semaphore)
{
	if (this->swapchain->invalid())
	{
		vector2U surface_size = this->window->getWindowSize();
		this->swapchain->recreateSwapchain({ surface_size.x, surface_size.y });
	}

	if (this->swapchain->invalid())
	{
		return;
	}

	VkResult result = vkAcquireNextImageKHR(this->device->get(), this->swapchain->get(), std::numeric_limits<uint64_t>::max(), signal_semaphore, VK_NULL_HANDLE, &image_index);

	if (result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		this->swapchain->invalidateSwapchain();
		vkDeviceWaitIdle(this->device->get()); // Temp Solution
	}
}

void VulkanInstance::cycleResourceDeleters()
{
	this->buffer_deleter->cycle();
	this->texture_deleter->cycle();
	this->view_deleter->cycle();
	this->shader_deleter->cycle();
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