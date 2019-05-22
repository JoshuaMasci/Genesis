#include "VulkanDevice.hpp"

#include "Genesis/Rendering/Vulkan/VulkanQueueFamily.hpp"
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

	VulkanQueueFamilyAllocator queue_allocator(this->physical_device, instance->surface);
	vector<VkDeviceQueueCreateInfo> queueCreateInfos;

	//Fill queue priorities with the required number of 1.0fs
	vector<float> queue_priority(1);
	for (QueueFamilyUseage queueFamily : queue_allocator.queue_families)
	{
		if (queue_priority.size() < queueFamily.queues_needed)
		{
			queue_priority.resize(queueFamily.queues_needed);
		}
	}

	for (uint32_t i = 0; i < queue_priority.size(); i++)
	{
		queue_priority[i] = 1.0f;
	}

	for (QueueFamilyUseage queueFamily : queue_allocator.queue_families)
	{
		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily.queue_family;
		queueCreateInfo.queueCount = queueFamily.queues_needed;
		queueCreateInfo.pQueuePriorities = queue_priority.data();
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

	vkGetDeviceQueue(this->logical_device, queue_allocator.graphics.queue_family, queue_allocator.graphics.queue_index, &this->graphics_queue);
	vkGetDeviceQueue(this->logical_device, queue_allocator.present.queue_family, queue_allocator.present.queue_index, &this->present_queue);
	vkGetDeviceQueue(this->logical_device, queue_allocator.transfer.queue_family, queue_allocator.transfer.queue_index, &this->transfer_queue);
	vkGetDeviceQueue(this->logical_device, queue_allocator.compute.queue_family, queue_allocator.compute.queue_index, &this->compute_queue);

	this->graphics_family_index = queue_allocator.graphics.queue_family;
	this->present_family_index = queue_allocator.present.queue_family;
	this->transfer_family_index = queue_allocator.transfer.queue_family;
	this->compute_family_index = queue_allocator.compute.queue_family;
}

VulkanDevice::~VulkanDevice()
{
	vkDestroyDevice(this->logical_device, nullptr);
}