#pragma once

#include "Genesis/Rendering/Vulkan/VulkanInclude.hpp"

namespace Genesis
{
	class VulkanDebugLayer
	{
	public:
		VulkanDebugLayer(VkInstance instance);
		~VulkanDebugLayer();
	private:
		VkInstance instance;
		VkDebugUtilsMessengerEXT debug_messenger = VK_NULL_HANDLE;
	};
}