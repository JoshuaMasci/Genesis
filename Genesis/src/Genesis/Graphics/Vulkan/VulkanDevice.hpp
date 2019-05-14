#pragma once

#include "Genesis/Graphics/Vulkan/VulkanInclude.hpp"

namespace Genesis
{
	class VulkanDevice
	{
	public:
		VulkanDevice(VkPhysicalDevice chosen_device);
		~VulkanDevice();
		
		inline VkDevice getDevice() { return this->logical_device; };
		
	private:
		//Device
		VkPhysicalDevice physical_device;
		VkDevice logical_device;
		
		//Device Properties
		VkPhysicalDeviceProperties properties;
		VkPhysicalDeviceMemoryProperties memory_properties;
		VkPhysicalDeviceFeatures features;
	};	
}