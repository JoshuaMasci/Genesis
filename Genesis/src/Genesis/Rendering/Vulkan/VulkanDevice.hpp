#pragma once

#include "Genesis/Rendering/Vulkan/VulkanInclude.hpp"

namespace Genesis
{
	class VulkanInstance;

	class VulkanDevice
	{
	public:
		VulkanDevice(VkPhysicalDevice chosen_device, VulkanInstance* instance);
		~VulkanDevice();
		
		inline VkDevice getDevice() { return this->logical_device; };
		inline VkPhysicalDevice getPhysicalDevice() { return this->physical_device; };

		inline VkCommandPool getGraphicsCommandPool() { return this->graphics_command_pool; };

		inline VkQueue getGraphicsQueue() { return this->graphics_queue; };
		inline VkQueue getPresentQueue() { return this->present_queue; };
		inline VkQueue getTransferQueue() { return this->transfer_queue; };
		inline VkQueue getComputeQueue() { return this->compute_queue; };

	private:
		//Device
		VkPhysicalDevice physical_device;
		VkDevice logical_device;
		
		//Device Properties
		VkPhysicalDeviceProperties properties;
		VkPhysicalDeviceMemoryProperties memory_properties;
		VkPhysicalDeviceFeatures features;

		VkQueue graphics_queue;
		VkQueue present_queue;
		VkQueue compute_queue;
		VkQueue transfer_queue;

		//CommandPools
		VkCommandPool graphics_command_pool;
	};	
}