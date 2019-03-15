#include "VulkanDevice.hpp"

#include "Genesis/Graphics/VulkanQueueFamilyIndices.hpp"

using namespace Genesis;

VulkanDevice::VulkanDevice(VulkanConfig& config, VulkanSurface* surface, VkPhysicalDevice physical_device)
{
	this->physical_device = physical_device;

	VkPhysicalDeviceProperties deviceProperties;
	vkGetPhysicalDeviceProperties(this->physical_device, &deviceProperties);
	printf("Device Name: %s\n", deviceProperties.deviceName);


	VulkanQueueFamilyIndices indices = VulkanQueueFamilyIndices::findQueueFamilies(this->physical_device, surface);

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

	VkDeviceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos = queueCreateInfos.data();

	createInfo.pEnabledFeatures = &deviceFeatures;

	createInfo.enabledExtensionCount = 0;

	vector<const char*> extensions = config.getRequiredDeviceExtensions();
	if (extensions.size() > 0)
	{
		createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		createInfo.ppEnabledExtensionNames = extensions.data();
	}
	else
	{
		createInfo.enabledExtensionCount = 0;
	}

	vector<const char*> layers = config.getRequiredLayers();
	if (layers.size() > 0)
	{
		createInfo.enabledLayerCount = static_cast<uint32_t>(layers.size());
		createInfo.ppEnabledLayerNames = layers.data();
	}
	else
	{
		createInfo.enabledLayerCount = 0;
	}

	if (vkCreateDevice(this->physical_device, &createInfo, nullptr, &this->device) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create logical device!");
	}

	vkGetDeviceQueue(this->device, indices.graphicsFamily.value(), 0, &this->graphicsQueue);
	vkGetDeviceQueue(this->device, indices.presentFamily.value(), 0, &this->presentQueue);
}

VulkanDevice::~VulkanDevice()
{
	vkDestroyDevice(this->device, nullptr);
}

VkPhysicalDevice VulkanDevice::getPhysicalDevice()
{
	return this->physical_device;
}

VkDevice VulkanDevice::getDevice()
{
	return this->device;
}
