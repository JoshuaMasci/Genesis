#pragma once

#include "Genesis/Core/Types.hpp"
#include <vulkan/vulkan.h>

namespace Genesis
{
	class VulkanConfig
	{
	public:
		void setRequiredExtensions(vector<const char*> extensions) { this->extensions = extensions; };
		void setRequiredDeviceExtensions(vector<const char*> extensions) { this->device_extensions = extensions; };
		void setRequiredLayers(vector<const char*> layers) { this->layers = layers; };

		vector<const char*> getRequiredExtensions() { return this->extensions; };
		vector<const char*> getRequiredDeviceExtensions() { return this->device_extensions; };
		vector<const char*> getRequiredLayers() { return this->layers; };

	private:
		vector<const char*> extensions;
		vector<const char*> device_extensions;
		vector<const char*> layers;
	};


	class VulkanInstance
	{
	public:
		VulkanInstance(VulkanConfig& config, const char* applicationName, uint32_t applicationVersion);
		~VulkanInstance();

		VkInstance getInstance();

	private:
		VkInstance instance;
	};
}