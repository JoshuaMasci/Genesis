#pragma once

#include "Genesis/Core/VectorTypes.hpp"
#include "Genesis/RenderingBackend/RenderingTypes.hpp"
#include "Genesis/RenderingBackend/RenderingBackend.hpp"

namespace Genesis
{
	class RenderLayer
	{
	public:
		RenderLayer(RenderingBackend* backend) 
		{
			this->backend = backend;
		};

		virtual void ImGuiDraw() = 0;

		virtual Framebuffer getFramebuffer() = 0;
		virtual uint32_t getFramebufferIndex() = 0;

	protected:
		RenderingBackend* backend = nullptr;
	};
}