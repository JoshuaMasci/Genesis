#pragma once

#include "Genesis/Rendering/Vulkan/VulkanInclude.hpp"
#include "Genesis/Core/Types.hpp"

namespace Genesis
{
	class VulkanDescriptorPool
	{
	public:
		VulkanDescriptorPool(VkDevice device, uint32_t frame_count, uint32_t max_sets, vector<VkDescriptorPoolSize> types);
		~VulkanDescriptorPool();

		inline VkDescriptorPool get() { return this->pool; };

		VkDescriptorSet getDescriptorSet(VkDescriptorSetLayout layout, uint32_t frame_index);

		void resetFrame(uint32_t frame_index);

	private:
		static const size_t MAX_PER_FRAME_DESCRIPTOR_SETS = 1000;

		VkDevice device;
		VkDescriptorPool pool;

		//Per frame descriptorset allocations
		struct FrameData
		{
			Array<VkDescriptorSet> used_descriptor_set;
			uint32_t descriptor_set_count = 0;
		};
		
		Array<FrameData> frame_data;
	};
}