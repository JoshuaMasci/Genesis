#pragma once

#include "Genesis/RenderingBackend/Sampler.hpp"
#include "VulkanInclude.hpp"
#include "Genesis/Core/Types.hpp"

#include <mutex>

namespace Genesis
{
	class VulkanSamplerPool
	{
	public:
		VulkanSamplerPool(VkDevice device);
		~VulkanSamplerPool();

		VkSampler getSampler(const SamplerCreateInfo& create_info);

	private:
		VkDevice device;

		std::mutex map_lock;
		map<uint32_t, VkSampler> samplers;
	};
}