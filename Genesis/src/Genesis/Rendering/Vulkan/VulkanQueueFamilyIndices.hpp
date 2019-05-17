#pragma once

#include "Genesis/Core/Types.hpp"

#include "Genesis/Rendering/Vulkan/VulkanInclude.hpp"
#include <optional>

namespace Genesis
{
	class VulkanSurface;

	class VulkanQueueFamilyIndices
	{
	public:
		std::optional<uint32_t> graphics_family;
		std::optional<uint32_t> present_family;
		std::optional<uint32_t> compute_family;

		bool hasGraphics() 
		{
			return graphics_family.has_value();
		}

		bool hasPresent()
		{
			return present_family.has_value();
		}

		bool hasCompute()
		{
			return compute_family.has_value();
		}

		static VulkanQueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);
	};

}