#pragma once

#include <cstdint>

namespace Genesis
{
	enum class MemoryUsage
	{
		GPU_Only,
		CPU_Visable
	};

	enum class ViewType
	{
		FrameBuffer,
		ShadowMap
	};

	typedef void* VertexBufferHandle;
	typedef void* IndexBufferHandle;
	typedef void* UniformBufferHandle;
	typedef void* TextureHandle;
	typedef void* ShaderHandle;

	typedef void* ViewHandle;
}