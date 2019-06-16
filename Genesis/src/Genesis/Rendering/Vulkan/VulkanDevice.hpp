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
		
		inline VkDevice get() { return this->logical_device; };
		inline VkPhysicalDevice getPhysicalDevice() { return this->physical_device; };

		inline VkQueue getGraphicsQueue() { return this->graphics_queue; };
		inline uint32_t getGraphicsFamilyIndex() { return this->graphics_family_index; };

		inline VkQueue getPresentQueue() { return this->present_queue; };
		inline uint32_t getPresentFamilyIndex() { return this->present_family_index; };

		inline VkQueue getTransferQueue() { return this->transfer_queue; };
		inline uint32_t getTransferFamilyIndex() { return this->transfer_family_index; };

		inline VkQueue getComputeQueue() { return this->compute_queue; };
		inline uint32_t getComputeFamilyIndex() { return this->compute_family_index; };

	private:
		//Device
		VkPhysicalDevice physical_device;
		VkDevice logical_device;
		
		//Device Properties
		VkPhysicalDeviceProperties properties;
		VkPhysicalDeviceMemoryProperties memory_properties;
		VkPhysicalDeviceFeatures features;

		VkQueue graphics_queue;
		uint32_t graphics_family_index;

		VkQueue present_queue;
		uint32_t present_family_index;

		VkQueue compute_queue;
		uint32_t compute_family_index;

		VkQueue transfer_queue;
		uint32_t transfer_family_index;
	};	
}