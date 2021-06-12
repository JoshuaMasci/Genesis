#pragma once

//#include "genesis_core/rendering/frame_graph.hpp"

namespace genesis
{
	enum class MemoryType
	{
		GpuOnly,
		CpuRead,
		CpuWrite,
	};

	typedef void* ShaderModule;
}