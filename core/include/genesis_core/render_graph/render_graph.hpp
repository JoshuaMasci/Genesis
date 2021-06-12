#pragma once

#include "genesis_core/types/containers.hpp"

namespace genesis
{
	typedef uint32_t ResourceId;
	const ResourceId NullResource = UINT32_MAX;

	enum class ResourceType 
	{
		Sync, //Arbitrary resource
		StorageBuffer, //All Gpu Buffers (Uniform, Vertex, etc...)
		SampledImage, //Images that will be sampled in a shader (e.g. Textures)
		StorageImage, //Images that will not be sampled, could be used for compute or blits
		MemoryBuffer, //Cpu memory for whatever cpu things you need
		AccelerationStructure, //Raytracing Tlas and Blas
	};
	
	enum class ResourceLifetime 
	{
		Permanent,
		Transient,
	};

	enum class ResourceAccess {
		ReadOnly,
		ReadWrite,
	};

	struct ResourceUsageInfo {
		ResourceId id;
		ResourceType type;
		ResourceAccess access;
	};

	enum class RenderTaskType {
		RenderPass,
		ComputePass,
	};

	struct RenderPassInfo {
		vector<ResourceId> color_attachmenets;
		ResourceId depth_attachment = NullResource;
	};

	struct RenderTask {
		RenderTaskType type;
		string name;
		vector<ResourceUsageInfo> resource_usage;
		RenderPassInfo render_pass_info;
	};
}