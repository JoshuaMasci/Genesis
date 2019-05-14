#include "VulkanContext.hpp"

#include "Genesis/Graphics/Vulkan/VulkanInstance.hpp"

using namespace Genesis;

VulkanContext::VulkanContext(Window* window)
{
	this->vulkan = new VulkanInstance(window);
}

VulkanContext::~VulkanContext()
{
	delete this->vulkan;
}
