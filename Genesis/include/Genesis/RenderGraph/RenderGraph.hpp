#pragma once

#include "Genesis/Core/Types.hpp"
#include "Genesis/RenderGraph/RenderPass.hpp"

namespace Genesis
{
	class RenderGraph
	{
	public:
		void addRenderPass(RenderPass* render_pass);

		void compile();

	protected:
		vector<RenderPass*> render_passes;
	};
}