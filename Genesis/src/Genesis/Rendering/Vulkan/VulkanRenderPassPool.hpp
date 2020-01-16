#pragma once

#include <mutex>

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

		VkRenderPass getRenderPass(uint32_t hash, List<VkFormat>& color_formats, VkFormat depth_format);

	private:
		std::mutex descriptor_map_lock;

		VkDevice device = VK_NULL_HANDLE;

		map<uint32_t, VkRenderPass> render_passes;
	};
}