#include "vulkan_renderer/renderer.hpp"

#include <stdio.h>

namespace genesis
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

	VkInstance create_instance(ApplicationInfo& app_info, Settings& settings, genesis::vector<const char*>& extensions, std::vector<const char*>& layers)
	{
		VkApplicationInfo appInfo = {};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = app_info.app_name;
		appInfo.applicationVersion = app_info.app_version;
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

	Renderer::Renderer(ApplicationInfo& app_info, Settings& settings, WindowInfo& window)
	{
		std::vector<const char*> extensions;
		extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
		extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
		extensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
#ifdef VK_USE_PLATFORM_WIN32_KHR
		extensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#endif

		std::vector<const char*> device_extensions;
		device_extensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
		device_extensions.push_back(VK_KHR_MAINTENANCE3_EXTENSION_NAME);
		device_extensions.push_back(VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME);

		std::vector<const char*> layers;
		layers.push_back("VK_LAYER_KHRONOS_validation");

		this->instance = create_instance(app_info, settings, extensions, layers);
		this->debug_layer.create(this->instance);		
		this->surface = create_surface(this->instance, window.handle);

		this->device = new Device(this->instance, get_physical_device(this->instance, this->surface), device_extensions, layers);
		this->swapchain = new Swapchain(this->device, this->surface, { window.width, window.height });

		this->wait_fence = this->device->create_fence();
		this->image_wait_semaphore = this->device->create_semaphore();
		this->present_wait_semaphore = this->device->create_semaphore();

		this->primary_command_pool = std::make_unique<CommandPool>(this->device->get(), this->device->get_queue_index(), VK_COMMAND_BUFFER_LEVEL_PRIMARY, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

		vector<VkDescriptorSetLayoutBinding> bindings = 
		{
			{0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, settings.max_storage_buffer_bindings, VK_SHADER_STAGE_ALL, nullptr},
			{1, VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, settings.max_sampled_image_bindings, VK_SHADER_STAGE_ALL, nullptr},
			{2, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, settings.max_storage_image_bindings, VK_SHADER_STAGE_ALL, nullptr},
		};
		this->bindless_descriptor = std::make_unique<BindlessDescriptor>(this->device->get(), bindings);
	}

	Renderer::~Renderer()
	{
		this->device->wait_idle();

		this->bindless_descriptor.reset();
		this->primary_command_pool.reset();

		this->device->destroy_semaphore(this->image_wait_semaphore);
		this->device->destroy_semaphore(this->present_wait_semaphore);
		this->device->destroy_fence(this->wait_fence);

		delete this->swapchain;
		delete this->device;

		vkDestroySurfaceKHR(this->instance, this->surface, nullptr);
		this->debug_layer.destroy();
		vkDestroyInstance(this->instance, nullptr);
	}

	VmaMemoryUsage get_memory_usage(MemoryType type)
	{
		switch (type)
		{
		case genesis::MemoryType::GpuOnly:
			return VmaMemoryUsage::VMA_MEMORY_USAGE_GPU_ONLY;
		case genesis::MemoryType::CpuRead:
			return VmaMemoryUsage::VMA_MEMORY_USAGE_GPU_TO_CPU;
		case genesis::MemoryType::CpuWrite:
			return VmaMemoryUsage::VMA_MEMORY_USAGE_CPU_TO_GPU;
		}
		return VmaMemoryUsage::VMA_MEMORY_USAGE_UNKNOWN;
	}

	VkFormat get_image_format(ImageFormat format)
	{
		switch (format)
		{
		case ImageFormat::RGBA_8_Unorm:
			return VK_FORMAT_R8G8B8A8_UNORM;
		case ImageFormat::R_16_Float:
			return VK_FORMAT_R16_SFLOAT;
		case ImageFormat::RG_16_Float:
			return VK_FORMAT_R16G16_SFLOAT;
		case ImageFormat::RGB_16_Float:
			return VK_FORMAT_R16G16B16_SFLOAT;
		case ImageFormat::RGBA_16_Float:
			return VK_FORMAT_R16G16B16A16_SFLOAT;
		case ImageFormat::R_32_Float:
			return VK_FORMAT_R32_SFLOAT;
		case ImageFormat::RG_32_Float:
			return VK_FORMAT_R32G32_SFLOAT;
		case ImageFormat::RGB_32_Float:
			return VK_FORMAT_R32G32B32_SFLOAT;
		case ImageFormat::RGBA_32_Float:
			return VK_FORMAT_R32G32B32A32_SFLOAT;
		case ImageFormat::D_16_Unorm:
			return VK_FORMAT_D16_UNORM;
		case ImageFormat::D_32_Float:
			return VK_FORMAT_D32_SFLOAT;
		}
		return VK_FORMAT_UNDEFINED;
	}

	size_t Renderer::create_image(const ImageCreateInfo& create_info)
	{
		VkImageCreateInfo image_info = {};
		image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		image_info.imageType = VK_IMAGE_TYPE_2D;
		image_info.extent.width = create_info.size.x;
		image_info.extent.height = create_info.size.y;
		image_info.extent.depth = 1;
		image_info.mipLevels = 1;
		image_info.arrayLayers = 1;
		image_info.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		image_info.format = get_image_format(create_info.format);
		image_info.tiling = create_info.optimal_tiling ? VK_IMAGE_TILING_OPTIMAL : VK_IMAGE_TILING_LINEAR;
		image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		image_info.samples = (VkSampleCountFlagBits)create_info.sampele_count;
		image_info.flags = 0;

		VkImage image;
		VmaAllocation allocation;
		VmaAllocationInfo allocation_info;

		this->device->create_image(&image_info, get_memory_usage(create_info.memory_type), &image, &allocation, &allocation_info);

		return size_t();
	}

	void Renderer::destroy_image(size_t id)
	{

	}

	void Renderer::draw_frame()
	{
		vkWaitForFences(this->device->get(), 1, &this->wait_fence, VK_TRUE, std::numeric_limits<uint64_t>::max());
		vkResetFences(this->device->get(), 1, &this->wait_fence);
		uint32_t image_index = this->swapchain->get_next_image(this->image_wait_semaphore);

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

		this->swapchain->present_image(image_index, this->present_wait_semaphore);
	}
}