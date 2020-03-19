#pragma once

#include "Genesis/Core/Types.hpp"
#include "Genesis/RenderingBackend/RenderingTypes.hpp"

namespace Genesis
{
	typedef void* DescriptorSetLayout;
	typedef void* DescriptorSet;

	enum class BindingType
	{
		Sampler,
		Sampled_Image,
		Storage_Image,
		Uniform_Buffer,
		Storage_Buffer
	};

	struct DescriptorBinding
	{
		BindingType binding_type;
		ShaderStage binding_stages;
	};

	struct DescriptorSetLayoutCreateInfo
	{
		DescriptorBinding* descriptor_bindings = nullptr;
		uint32_t descriptor_bindings_count = 0;
	};

	struct DescriptorSetBinding
	{
		BindingType type;
		void* handle;
	};

	struct DescriptorSetCreateInfo
	{
		DescriptorSetLayout layout = nullptr;
		DescriptorSetBinding* descriptor_bindings = nullptr;
		uint32_t descriptor_bindings_count = 0;
	};
}