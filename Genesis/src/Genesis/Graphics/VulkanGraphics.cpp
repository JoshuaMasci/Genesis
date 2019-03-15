#include "VulkanGraphics.hpp"

#include "Genesis/Platform/Window.hpp"
#include "Genesis/Graphics/VulkanPhysicalDevicePicker.hpp"

using namespace Genesis;

VulkanGraphics::VulkanGraphics(Window* window)
	:window(window)
{
	this->initVulkan();
}

VulkanGraphics::~VulkanGraphics()
{
	this->destroyVulkan();
}

void VulkanGraphics::initVulkan()
{
	this->config.setRequiredExtensions(this->getRequiredExtensions());
	this->config.setRequiredDeviceExtensions({ VK_KHR_SWAPCHAIN_EXTENSION_NAME });
	this->config.setRequiredLayers(this->getRequiredLayers());

	this->instance = new VulkanInstance(this->config, "Sandbox", VK_MAKE_VERSION(0, 0, 0));
	this->surface = new VulkanSurface(this->instance, this->window);

	if (this->debug_validation_layers)
	{
		this->debugMessenger = new VulkanDebugMessenger(this->instance);
	}

	VulkanPhysicalDevicePicker picker = VulkanPhysicalDevicePicker(this->instance, this->surface);
	this->device = new VulkanDevice(this->config, this->surface, picker.pickDevice());

	this->swapChain = new VulkanSwapChain(this->device, this->surface, this->window);

	this->imageViews = new VulkanImageViews(this->device, this->swapChain);
}


void VulkanGraphics::destroyVulkan()
{
	delete this->imageViews;
	delete this->swapChain;
	delete this->device;

	if (this->debug_validation_layers)
	{
		delete this->debugMessenger;
	}

	delete this->surface;
	delete this->instance;
}

vector<const char*> VulkanGraphics::getRequiredExtensions()
{
	vector<const char*> extensions;

	extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
	extensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);

	if (this->debug_validation_layers)
	{
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
	}

	return extensions;
}

vector<const char*> VulkanGraphics::getRequiredLayers()
{
	vector<const char*> layers;

	if (this->debug_validation_layers)
	{
		layers.push_back("VK_LAYER_LUNARG_standard_validation");
	}

	return layers;
}