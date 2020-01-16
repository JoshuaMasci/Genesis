#include "VulkanQueueFamily.hpp"

#include "vulkan/vulkan.h"

using namespace Genesis;

VulkanSupportedQueues::VulkanSupportedQueues(VkPhysicalDevice device, VkSurfaceKHR surface)
{
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
	vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	for (uint32_t i = 0; i < queueFamilies.size(); i++)
	{
		const auto& queueFamily = queueFamilies[i];

		if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			this->flags[(uint32_t)QueueFlags::Graphics] = true;
		}

		if (surface != VK_NULL_HANDLE)
		{
			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

			if (queueFamily.queueCount > 0 && presentSupport)
			{
				this->flags[(uint32_t)QueueFlags::Present] = true;
			}
		}

		if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT)
		{
			this->flags[(uint32_t)QueueFlags::Transfer] = true;
		}

		if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT)
		{
			this->flags[(uint32_t)QueueFlags::Compute] = true;
		}
	}
}

bool VulkanSupportedQueues::hasFullSupport()
{
	return this->flags[0] && this->flags[1] && this->flags[2] && this->flags[3];
}

VulkanQueueFamilyDetails::VulkanQueueFamilyDetails(VkPhysicalDevice device, VkSurfaceKHR surface)
{
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
	vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());
	
	this->queue_families.resize(queueFamilyCount);

	for (uint32_t i = 0; i < queueFamilies.size(); i++)
	{
		const auto& queueFamily = queueFamilies[i];

		this->queue_families[i].queue_count = queueFamily.queueCount;

		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			this->queue_families[i].flags[(uint32_t)QueueFlags::Graphics] = true;
		}

		if (surface != nullptr)
		{
			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

			if (presentSupport)
			{
				this->queue_families[i].flags[(uint32_t)QueueFlags::Present] = true;
			}
		}

		if (queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT)
		{
			this->queue_families[i].flags[(uint32_t)QueueFlags::Transfer] = true;
		}

		if (queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT)
		{
			this->queue_families[i].flags[(uint32_t)QueueFlags::Compute] = true;
		}
	}
}

QueueSelection findQueue(VulkanQueueFamilyDetails& details, vector<uint32_t>& used_queues, uint32_t flag)
{
	QueueSelection result;

	//Check for avalible queue first
	for (uint32_t i = 0; i < details.queue_families.size(); i++)
	{
		QueueFamily famliy = details.queue_families[i];

		if (famliy.flags[flag])
		{
			if (famliy.queue_count > used_queues[i])
			{
				result.queue_family = i;
				result.queue_index = used_queues[i];
				used_queues[i]++;
				return result;
			}
		}
	}

	//Else look for queue to share
	//always share the 0th queue
	for (uint32_t i = 0; i < details.queue_families.size(); i++)
	{
		QueueFamily famliy = details.queue_families[i];

		if (famliy.flags[flag])
		{
			result.queue_family = i;
			result.queue_index = 0;
			return result;
		}
	}

	//Should Error
	return result;
}

VulkanQueueFamilyAllocator::VulkanQueueFamilyAllocator(VkPhysicalDevice device, VkSurfaceKHR surface)
{
	VulkanQueueFamilyDetails queue_family_details(device, surface);
	vector<uint32_t> used_queues(queue_family_details.queue_families.size());
	for (uint32_t i = 0; i < used_queues.size(); i++)
	{
		used_queues[i] = 0;
	}

	//Select Graphics first
	this->graphics = findQueue(queue_family_details, used_queues, (uint32_t)QueueFlags::Graphics);

	//Select Present Second
	this->present = findQueue(queue_family_details, used_queues, (uint32_t)QueueFlags::Present);

	//Select Transfer Third
	this->transfer = findQueue(queue_family_details, used_queues, (uint32_t)QueueFlags::Transfer);

	//Select Compute Last
	this->compute = findQueue(queue_family_details, used_queues, (uint32_t)QueueFlags::Compute);

	//Calculate Queue Families used
	for (uint32_t i = 0; i < used_queues.size(); i++)
	{
		if (used_queues[i] > 0)
		{
			this->queue_families.push_back({i, used_queues[i]});
		}
	}
}
