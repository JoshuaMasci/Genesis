#pragma once

#include "Genesis/Core/Types.hpp"

#include "vulkan/vulkan.h"
#include <optional>

namespace Genesis
{
	class VulkanSurface;

	class VulkanQueueFamilyIndices
	{
	public:
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;

		bool isComplete() {
			return graphicsFamily.has_value() && presentFamily.has_value();
		}

		static VulkanQueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);
	};

}