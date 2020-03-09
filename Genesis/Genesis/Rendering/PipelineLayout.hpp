#pragma once

#include "Genesis/Core/Types.hpp"
#include "Genesis/Rendering/DescriptorSet.hpp"

namespace Genesis
{
	typedef void* PipelineLayout;

	struct PushConstantRange
	{
		ShaderStage stages;
		uint32_t offset;
		uint32_t size;
	};

	struct PipelineLayoutCreateInfo
	{
		DescriptorSetLayout* descriptor_sets = nullptr;
		uint32_t descriptor_sets_count = 0;

		PushConstantRange* push_constant_ranges = nullptr;
		uint32_t push_constant_ranges_count = 0;
	};
}