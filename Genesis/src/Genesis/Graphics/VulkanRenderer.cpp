#include "VulkanRenderer.hpp"

using namespace Genesis;

#include "Genesis/Graphics/VulkanPhysicalDevicePicker.hpp"
#include "Genesis/Graphics/VulkanQueueFamilyIndices.hpp"

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
}

VulkanRenderer::~VulkanRenderer()
{
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
