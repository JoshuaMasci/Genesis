#include "VulkanPhysicalDevicePicker.hpp"

#include "Genesis/Debug/Assert.hpp"

#include "VulkanQueueFamily.hpp"

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

	GENESIS_ENGINE_ASSERT_ERROR((deviceCount != 0), "failed to find GPUs with Vulkan support");

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

	GENESIS_ENGINE_ASSERT_ERROR(physicalDevice != VK_NULL_HANDLE, "failed to find a suitable GPU");

	return physicalDevice;
}
