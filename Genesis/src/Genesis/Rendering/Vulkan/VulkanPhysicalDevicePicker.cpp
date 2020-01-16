#include "VulkanPhysicalDevicePicker.hpp"

#include "Genesis/Rendering/Vulkan/VulkanQueueFamily.hpp"

using namespace Genesis;

bool isDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface)
{
	VulkanSupportedQueues supported_queues(device, surface);

	return supported_queues.hasFullSupport();
}

VkPhysicalDevice VulkanPhysicalDevicePicker::pickDevice(VkInstance instance, VkSurfaceKHR surface)
{
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

	if (deviceCount == 0)
	{
		throw std::runtime_error("failed to find GPUs with Vulkan support!");
	}

	vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

	for (const auto& temp_device : devices)
	{
		if (isDeviceSuitable(temp_device, surface))
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
