#pragma once

#include "Genesis/Graphics/VulkanInstance.hpp"
#include "Genesis/Graphics/VulkanSurface.hpp"

namespace Genesis
{
	class VulkanDevice
	{
	public:
		VulkanDevice(VulkanConfig& config, VulkanSurface* surface, VkPhysicalDevice physical_device);
		~VulkanDevice();

		VkPhysicalDevice getPhysicalDevice();
		VkDevice getDevice();

	private:
		VkPhysicalDevice physical_device;
		VkDevice device;

		VkQueue graphicsQueue;
		VkQueue presentQueue;
	};
}