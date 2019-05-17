#include "VulkanDevice.hpp"

#include "Genesis/Rendering/Vulkan/VulkanQueueFamilyIndices.hpp"
#include "Genesis/Rendering/Vulkan/VulkanInstance.hpp"

using namespace Genesis;

VulkanDevice::VulkanDevice(VkPhysicalDevice chosen_device, VulkanInstance* instance)
{
	this->physical_device = chosen_device;
	vkGetPhysicalDeviceProperties(this->physical_device, &this->properties);
	vkGetPhysicalDeviceMemoryProperties(this->physical_device, &this->memory_properties);
	vkGetPhysicalDeviceFeatures(this->physical_device, &this->features);

	printf("GPU INFO:\n");
	printf("Device: %s\n", this->properties.deviceName);

	VulkanQueueFamilyIndices indices = VulkanQueueFamilyIndices::findQueueFamilies(this->physical_device, instance->surface);
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::vector<uint32_t> uniqueQueueFamilies;
	if (indices.hasGraphics()) { uniqueQueueFamilies.push_back(indices.graphics_family.value()); };
	if (indices.hasPresent()) { uniqueQueueFamilies.push_back(indices.present_family.value()); };
	//if (indices.hasCompute()) { uniqueQueueFamilies.push_back(indices.compute_family.value()); };

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

	VkPhysicalDeviceFeatures device_features = {};
	//No device features needed as of now
	//TODO add as needed

	VkDeviceCreateInfo create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

	create_info.pQueueCreateInfos = queueCreateInfos.data();
	create_info.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());

	create_info.pEnabledFeatures = &device_features;

	create_info.enabledExtensionCount = 0;

	vector<const char*> extensions = instance->getDeviceExtensions();
	if (extensions.size() > 0)
	{
		create_info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		create_info.ppEnabledExtensionNames = extensions.data();
	}
	else
	{
		create_info.enabledExtensionCount = 0;
	}

	vector<const char*> layers = instance->getLayers();
	if (layers.size() > 0)
	{
		create_info.enabledLayerCount = static_cast<uint32_t>(layers.size());
		create_info.ppEnabledLayerNames = layers.data();
	}
	else
	{
		create_info.enabledLayerCount = 0;
	}

	if (vkCreateDevice(this->physical_device, &create_info, nullptr, &this->logical_device) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create logical device!");
	}

	vkGetDeviceQueue(this->logical_device, indices.graphics_family.value(), 0, &this->graphics_queue);
	vkGetDeviceQueue(this->logical_device, indices.present_family.value(), 0, &this->present_queue);

	/*if (indices.hasCompute())
	{
		vkGetDeviceQueue(this->logical_device, indices.present_family.value(), 0, &this->present_queue);
	}*/

	VkCommandPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = indices.graphics_family.value();
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	if (vkCreateCommandPool(this->logical_device, &poolInfo, nullptr, &this->graphics_command_pool) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create command pool!");
	}
}

VulkanDevice::~VulkanDevice()
{
	vkDestroyDevice(this->logical_device, nullptr);
}
