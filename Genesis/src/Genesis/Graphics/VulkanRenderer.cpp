#include "VulkanRenderer.hpp"

using namespace Genesis;

#include "Genesis/Graphics/VulkanPhysicalDevicePicker.hpp"
#include "Genesis/Graphics/VulkanQueueFamilyIndices.hpp"
#include "Genesis/Graphics/VulkanSwapChainSupportDetails.hpp"

#include <algorithm>

VulkanRenderer::VulkanRenderer(Window* window, const char* app_name)
{
	this->window = window;

	this->create_instance(app_name, VK_MAKE_VERSION(0, 0, 1));

	if (this->use_debug_layers)
	{
		this->create_debug_messenger();
	}

	this->create_surface();

	this->create_device_and_queues(VulkanPhysicalDevicePicker::pickDevice(this->context.instance, this->context.surface));

	this->create_swapchain();
}

VulkanRenderer::~VulkanRenderer()
{
	this->delete_swapchain();

	this->delete_device_and_queues();

	this->delete_surface();

	if (this->use_debug_layers)
	{
		this->delete_debug_messenger();
	}

	this->delete_instance();
}

void VulkanRenderer::create_instance(const char* app_name, uint32_t app_version)
{
	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = app_name;
	appInfo.applicationVersion = app_version;
	appInfo.pEngineName = "Genesis";
	appInfo.engineVersion = VK_MAKE_VERSION(0, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_1;

	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	vector<const char*> extensions = this->getExtensions();
	if (extensions.size() > 0)
	{
		createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		createInfo.ppEnabledExtensionNames = extensions.data();
	}
	else
	{
		createInfo.enabledExtensionCount = 0;
	}

	vector<const char*> layers = this->getLayers();
	if (layers.size() > 0)
	{
		createInfo.enabledLayerCount = static_cast<uint32_t>(layers.size());
		createInfo.ppEnabledLayerNames = layers.data();
	}
	else
	{
		createInfo.enabledLayerCount = 0;
	}

	VkResult result = vkCreateInstance(&createInfo, nullptr, &this->context.instance);

	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create Vulkan Instance!");
	}
}

void VulkanRenderer::delete_instance()
{
	vkDestroyInstance(this->context.instance, nullptr);
}

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
{
	printf("Vulkan Validation Layer: %s\n", pCallbackData->pMessage);
	return VK_FALSE;
}

void Genesis::VulkanRenderer::create_debug_messenger()
{
	VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = debugCallback;

	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(this->context.instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr)
	{
		VkResult result = func(this->context.instance, &createInfo, nullptr, &this->context.debug_messenger);
		if (result != VK_SUCCESS)
		{
			throw std::runtime_error("failed to set up debug messenger!");
		}
	}
	else
	{
		throw std::runtime_error("failed to set up debug messenger!");
	}
}

void Genesis::VulkanRenderer::delete_debug_messenger()
{
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(this->context.instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr)
	{
		func(this->context.instance, this->context.debug_messenger, nullptr);
	}
}

void Genesis::VulkanRenderer::create_surface()
{
#ifdef WIN32
	VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = { VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR };

	surfaceCreateInfo.hinstance = GetModuleHandle(0);
	surfaceCreateInfo.hwnd = (HWND)this->window->getNativeWindowHandle();

	VkResult result = vkCreateWin32SurfaceKHR(this->context.instance, &surfaceCreateInfo, NULL, &this->context.surface);

	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("failed to attach to surface");
	}
#endif // DEBUG
}

void Genesis::VulkanRenderer::delete_surface()
{
	vkDestroySurfaceKHR(this->context.instance, this->context.surface, nullptr);
}

void Genesis::VulkanRenderer::create_device_and_queues(VkPhysicalDevice physical_device)
{
	this->context.device_properties.physical_device = physical_device;
	vkGetPhysicalDeviceProperties(this->context.device_properties.physical_device, &this->context.device_properties.properties);
	vkGetPhysicalDeviceMemoryProperties(this->context.device_properties.physical_device, &this->context.device_properties.memory_properties);
	vkGetPhysicalDeviceFeatures(this->context.device_properties.physical_device, &this->context.device_properties.features);
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(this->context.device_properties.physical_device, this->context.surface, &this->context.device_properties.surface_capabilities);

	VulkanQueueFamilyIndices indices = VulkanQueueFamilyIndices::findQueueFamilies(this->context.device_properties.physical_device, this->context.surface);
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };

	float queuePriority = 1.0f;
	for (uint32_t queueFamily : uniqueQueueFamilies)
	{
		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	VkPhysicalDeviceFeatures deviceFeatures = {};
	//No device features needed as of now
	//TODO add as needed

	VkDeviceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos = queueCreateInfos.data();

	createInfo.pEnabledFeatures = &deviceFeatures;

	createInfo.enabledExtensionCount = 0;

	vector<const char*> extensions = this->getDeviceExtensions();
	if (extensions.size() > 0)
	{
		createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		createInfo.ppEnabledExtensionNames = extensions.data();
	}
	else
	{
		createInfo.enabledExtensionCount = 0;
	}

	vector<const char*> layers = this->getLayers();
	if (layers.size() > 0)
	{
		createInfo.enabledLayerCount = static_cast<uint32_t>(layers.size());
		createInfo.ppEnabledLayerNames = layers.data();
	}
	else
	{
		createInfo.enabledLayerCount = 0;
	}

	if (vkCreateDevice(this->context.device_properties.physical_device, &createInfo, nullptr, &this->context.device) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create logical device!");
	}

	vkGetDeviceQueue(this->context.device, indices.graphicsFamily.value(), 0, &this->context.graphics_queue);
	vkGetDeviceQueue(this->context.device, indices.presentFamily.value(), 0, &this->context.present_queue);
}

void Genesis::VulkanRenderer::delete_device_and_queues()
{
	vkDestroyDevice(this->context.device, nullptr);
}


//Swapchain UTILS
VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
	if (availableFormats.size() == 1 && availableFormats[0].format == VK_FORMAT_UNDEFINED)
	{
		return { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
	}

	for (const auto& availableFormat : availableFormats)
	{
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
		{
			return availableFormat;
		}
	}

	return availableFormats[0];
}

VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes)
{
	VkPresentModeKHR bestMode = VK_PRESENT_MODE_FIFO_KHR;

	for (const auto& availablePresentMode : availablePresentModes)
	{
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
		{
			return availablePresentMode;
		}
		else if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR)
		{
			bestMode = availablePresentMode;
		}
	}

	return bestMode;
}

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

void Genesis::VulkanRenderer::create_swapchain()
{
	VulkanSwapChainSupportDetails swapChainSupport = VulkanSwapChainSupportDetails::querySwapChainSupport(this->context.device_properties.physical_device, this->context.surface);

	VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
	VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);

	vector2U window_size = window->getWindowSize();
	VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities, { window_size.x, window_size.y });

	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
	if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
	{
		imageCount = swapChainSupport.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = this->context.surface;

	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	VulkanQueueFamilyIndices indices = VulkanQueueFamilyIndices::findQueueFamilies(this->context.device_properties.physical_device, this->context.surface);
	uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

	if (indices.graphicsFamily != indices.presentFamily)
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0;
		createInfo.pQueueFamilyIndices = nullptr;
	}

	createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;
	createInfo.oldSwapchain = VK_NULL_HANDLE;

	VkSwapchainKHR swapChain;
	VkResult result = vkCreateSwapchainKHR(this->context.device, &createInfo, nullptr, &swapChain);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create swap chain!");
	}

	vkGetSwapchainImagesKHR(this->context.device, this->context.swapchain, &imageCount, nullptr);
	this->context.swapchain_images.swapchain_images.resize(imageCount);
	vkGetSwapchainImagesKHR(this->context.device, this->context.swapchain, &imageCount, this->context.swapchain_images.swapchain_images.data());

	this->context.swapchain_properties.swapchain_image_format = surfaceFormat.format;
	this->context.swapchain_properties.swapchain_extent = extent;


	this->context.swapchain_images.swapchain_imageviews.resize(this->context.swapchain_images.swapchain_images.size());
	for (size_t i = 0; i < this->context.swapchain_images.swapchain_images.size(); i++)
	{
		VkImageViewCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = this->context.swapchain_images.swapchain_images[i];
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = this->context.swapchain_properties.swapchain_image_format;
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		if (vkCreateImageView(this->context.device, &createInfo, nullptr, &this->context.swapchain_images.swapchain_imageviews[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create image views!");
		}
	}
}

void Genesis::VulkanRenderer::delete_swapchain()
{
	vkDestroySwapchainKHR(this->context.device, this->context.swapchain, nullptr);

	for (auto imageView : this->context.swapchain_images.swapchain_imageviews)
	{
		vkDestroyImageView(this->context.device, imageView, nullptr);
	}
}

vector<const char*> Genesis::VulkanRenderer::getExtensions()
{
	vector<const char*> extensions;

	extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
	extensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);

	if (this->use_debug_layers)
	{
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
	}

	return extensions;
}

vector<const char*> Genesis::VulkanRenderer::getDeviceExtensions()
{
	return vector<const char*>();
}

vector<const char*> Genesis::VulkanRenderer::getLayers()
{
	vector<const char*> layers;

	if (this->use_debug_layers)
	{
		layers.push_back("VK_LAYER_LUNARG_standard_validation");
	}

	return layers;
}
