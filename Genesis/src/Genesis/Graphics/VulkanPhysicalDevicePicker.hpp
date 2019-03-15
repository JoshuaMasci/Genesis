#pragma once

#include "Genesis/Graphics/VulkanInstance.hpp"
#include "Genesis/Graphics/VulkanSurface.hpp"

namespace Genesis
{
	class VulkanPhysicalDevicePicker
	{
	public:
		VulkanPhysicalDevicePicker(VulkanInstance* instance, VulkanSurface* surface);
		~VulkanPhysicalDevicePicker();

		VkPhysicalDevice pickDevice();

	private:
		VulkanInstance* instance = nullptr;
		VulkanSurface* surface = nullptr;
	};
}