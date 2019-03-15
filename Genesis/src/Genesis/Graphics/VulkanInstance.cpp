#include "VulkanInstance.hpp"

using namespace Genesis;

VulkanInstance::VulkanInstance(VulkanConfig& config, const char* applicationName, uint32_t applicationVersion)
{
	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = applicationName;
	appInfo.applicationVersion = applicationVersion;
	appInfo.pEngineName = "Genesis";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	vector<const char*> extensions = config.getRequiredExtensions();
	if (extensions.size() > 0)
	{
		createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		createInfo.ppEnabledExtensionNames = extensions.data();
	}
	else
	{
		createInfo.enabledExtensionCount = 0;
	}

	vector<const char*> layers = config.getRequiredLayers();
	if (layers.size() > 0)
	{
		createInfo.enabledLayerCount = static_cast<uint32_t>(layers.size());
		createInfo.ppEnabledLayerNames = layers.data();
	}
	else
	{
		createInfo.enabledLayerCount = 0;
	}

	VkResult result = vkCreateInstance(&createInfo, nullptr, &this->instance);

	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create Vulkan Instance!");
	}
}

VulkanInstance::~VulkanInstance()
{
	vkDestroyInstance(this->instance, nullptr);
}

VkInstance VulkanInstance::getInstance()
{
	return this->instance;
}
