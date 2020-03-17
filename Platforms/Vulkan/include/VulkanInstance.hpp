#pragma once

#include "VulkanInclude.hpp"
#include "Genesis/Core/Types.hpp"

namespace Genesis
{
	struct VulkanInstance
	{
		static VkInstance create(uint32_t vulkan_version, const char* app_name, uint32_t app_version, const char* engine_name, uint32_t engine_version, vector<const char*>& extensions, vector<const char*>& layers);
	};

	//Prototype
	class Window;
	struct VulkanSurface
	{
		static VkSurfaceKHR create(VkInstance instance, Window* window);
	};
}