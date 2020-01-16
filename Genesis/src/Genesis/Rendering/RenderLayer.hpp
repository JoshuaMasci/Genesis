#pragma once

#include "Genesis/Core/VectorTypes.hpp"
#include "Genesis/Rendering/RenderingTypes.hpp"
#include "Genesis/Rendering/RenderingBackend.hpp"

namespace Genesis
{
	class RenderLayer
	{
	public:
		RenderLayer(RenderingBackend* backend) 
		{
			this->backend = backend;
		};

		virtual void startLayer() = 0;
		virtual void endLayer() = 0;

		virtual View getView() = 0;
		virtual uint32_t getViewImageIndex() = 0;

	protected:
		RenderingBackend* backend = nullptr;
	};
}