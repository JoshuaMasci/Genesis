#pragma once

#include "genesis_core/rendering/frame_graph.hpp"

namespace genesis_core
{
	class RenderingBackend
	{
	public:
		virtual ~RenderingBackend() = 0;

		virtual void render(FrameGraph* frame_graph) = 0;
	};
}