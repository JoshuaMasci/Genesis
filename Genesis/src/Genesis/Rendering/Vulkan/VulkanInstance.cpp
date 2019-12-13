#include "VulkanInstance.hpp"

VkInstance Genesis::VulkanInstance::create(uint32_t vulkan_version, const char* app_name, uint32_t app_version, const char* engine_name, uint32_t engine_version, vector<const char*>& extensions, vector<const char*>& layers)
{
	VkInstance instance = VK_NULL_HANDLE;

	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = app_name;
	appInfo.applicationVersion = app_version;
	appInfo.pEngineName = engine_name;
	appInfo.engineVersion = engine_version;
	appInfo.apiVersion = vulkan_version;

	VkInstanceCreateInfo create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	create_info.pApplicationInfo = &appInfo;

	if (extensions.size() > 0)
	{
		create_info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		create_info.ppEnabledExtensionNames = extensions.data();
	}
	else
	{
		create_info.enabledExtensionCount = 0;
	}

	if (layers.size() > 0)
	{
		create_info.enabledLayerCount = static_cast<uint32_t>(layers.size());
		create_info.ppEnabledLayerNames = layers.data();
	}
	else
	{
		create_info.enabledLayerCount = 0;
	}

	VkResult result = vkCreateInstance(&create_info, nullptr, &instance);

	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create Vulkan Instance!");
	}

	return instance;
}

#include "Genesis/Platform/Window.hpp"

VkSurfaceKHR Genesis::VulkanSurface::create(VkInstance instance, Window* window)
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
