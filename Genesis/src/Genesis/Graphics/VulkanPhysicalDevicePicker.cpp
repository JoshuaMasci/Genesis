#include "VulkanPhysicalDevicePicker.hpp"

#include "Genesis/Graphics/VulkanQueueFamilyIndices.hpp"
#include "Genesis/Graphics/VulkanSwapChainSupportDetails.hpp"

using namespace Genesis;

VulkanPhysicalDevicePicker::VulkanPhysicalDevicePicker(VulkanInstance* instance, VulkanSurface* surface)
{
	this->instance = instance;
	this->surface = surface;
}

VulkanPhysicalDevicePicker::~VulkanPhysicalDevicePicker()
{
	//Nothing
}

bool isDeviceSuitable(VkPhysicalDevice device, VulkanSurface* surface)
{
	VulkanQueueFamilyIndices indices = VulkanQueueFamilyIndices::findQueueFamilies(device, surface);

	bool extensionsSupported = true;//TODO checkDeviceExtensionSupport(device);

	bool swapChainAdequate = false;
	if (extensionsSupported) 
	{
		VulkanSwapChainSupportDetails swapChainSupport = VulkanSwapChainSupportDetails::querySwapChainSupport(device, surface);
		swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
	}

	return indices.isComplete() && extensionsSupported && swapChainAdequate;

	return indices.isComplete();
}

VkPhysicalDevice VulkanPhysicalDevicePicker::pickDevice()
{
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(this->instance->getInstance(), &deviceCount, nullptr);

	if (deviceCount == 0)
	{
		throw std::runtime_error("failed to find GPUs with Vulkan support!");
	}

	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(this->instance->getInstance(), &deviceCount, devices.data());

	for (const auto& temp_device : devices)
	{
		if (isDeviceSuitable(temp_device, this->surface))
		{
			physicalDevice = temp_device;
			break;
		}
	}

	if (physicalDevice == VK_NULL_HANDLE)
	{
		throw std::runtime_error("failed to find a suitable GPU!");
	}

	return physicalDevice;
}
