#pragma once

#include "Genesis/Graphics/VulkanInstance.hpp"
#include "Genesis/Platform/Window.hpp"
namespace Genesis
{
	class VulkanDebugMessenger
	{
	public:
		VulkanDebugMessenger(VulkanInstance* instance);
		~VulkanDebugMessenger();

	private:
		VkDebugUtilsMessengerEXT debugMessenger;
		VulkanInstance* instance;
	};
}