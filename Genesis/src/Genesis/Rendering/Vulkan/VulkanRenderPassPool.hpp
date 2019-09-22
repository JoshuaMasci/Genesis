#pragma once

#include "Genesis/Core/Types.hpp"
#include "Genesis/Rendering/Vulkan/VulkanInclude.hpp"
#include "Genesis/Rendering/Vulkan/VulkanDevice.hpp"
#include "Genesis/Rendering/FramebufferLayout.hpp"

namespace Genesis
{

	class VulkanRenderPassPool
	{
	public:
		VulkanRenderPassPool(VkDevice device);
		~VulkanRenderPassPool();

		VkRenderPass getRenderPass(uint32_t hash, Array<VkFormat>& color_formats, VkFormat depth_format);

	private:
		VkDevice device = VK_NULL_HANDLE;

		map <uint32_t, VkRenderPass> render_passes;
	};
}