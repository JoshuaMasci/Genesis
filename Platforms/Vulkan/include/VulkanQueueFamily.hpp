#pragma once

#include "Genesis/Core/Types.hpp"

#include "VulkanInclude.hpp"

namespace Genesis
{

	//Util classes
	enum class QueueFlags
	{
		Graphics = 0,
		Present = 1,
		Transfer = 2,
		Compute = 3,
	};

	struct QueueFamily
	{
		uint32_t queue_count = 0;
		bool flags[4] =
		{
			false,
			false,
			false,
			false
		};
	};

	struct QueueSelection
	{
		uint32_t queue_family = 0xffffffff;
		uint32_t queue_index = 0xffffffff;
	};

	struct QueueFamilyUseage
	{
		uint32_t queue_family = 0xffffffff;
		uint32_t queues_needed = 0;
	};

	//Used to choose device
	//Just need to see the queues are supported
	class VulkanSupportedQueues
	{
	public:
		VulkanSupportedQueues(VkPhysicalDevice device, VkSurfaceKHR surface);

		bool hasFullSupport();

		bool flags[4] =
		{
			false,
			false,
			false,
			false
		};
	};

	//Used to decide what queues will be used
	//Provides more information than VulkanSupportedQueues
	class VulkanQueueFamilyDetails
	{
	public:
		VulkanQueueFamilyDetails(VkPhysicalDevice device, VkSurfaceKHR surface);

		vector<QueueFamily> queue_families;
	};

	//Decides which queue will be used for each of the 4 required queue types
	//Dependent on the hardware, any number of the 4 queue tyepes may have to share
	class VulkanQueueFamilyAllocator
	{
	public:
		VulkanQueueFamilyAllocator(VkPhysicalDevice device, VkSurfaceKHR surface);

		vector<QueueFamilyUseage> queue_families;
		QueueSelection graphics; //Queue Info for graphics queue
		QueueSelection present; //Queue Info for present queue
		QueueSelection transfer; //Queue Info for transfer queue
		QueueSelection compute; //Queue Info for compute queue
	};
}