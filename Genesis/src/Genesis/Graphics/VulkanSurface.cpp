#include "VulkanSurface.hpp"

using namespace Genesis;

VulkanSurface::VulkanSurface(VulkanInstance* instance, Window* window)
{
	this->instance = instance;

	//Drawing Surface
	VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = { VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR };

	surfaceCreateInfo.hinstance = GetModuleHandle(0);
	surfaceCreateInfo.hwnd = (HWND)window->getNativeWindowHandle();

	VkResult result = vkCreateWin32SurfaceKHR(this->instance->getInstance(), &surfaceCreateInfo, NULL, &this->surface);

	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("failed to attach to surface");
	}
}

VulkanSurface::~VulkanSurface()
{
	vkDestroySurfaceKHR(this->instance->getInstance(), this->surface, nullptr);
}

VkSurfaceKHR VulkanSurface::getSurface()
{
	return this->surface;
}
