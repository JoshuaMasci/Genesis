#pragma once

#include "Genesis/Rendering/Sampler.hpp"
#include "Genesis/Rendering/Vulkan/VulkanInclude.hpp"
#include "Genesis/Core/Types.hpp"

#include <mutex>

namespace Genesis
{
	class VulkanSamplerPool
	{
	public:
		VulkanSamplerPool(VkDevice device);
		~VulkanSamplerPool();

		VkSampler getSampler(Sampler& sampler);

	private:
		VkDevice device;

		std::mutex map_lock;
		map<uint32_t, VkSampler> samplers;
	};
}