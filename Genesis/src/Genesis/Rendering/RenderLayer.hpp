#pragma once

#include "Genesis/Core/VectorTypes.hpp"
#include "Genesis/Rendering/RenderingTypes.hpp"

namespace Genesis
{
	class RenderLayer
	{
	public:
		virtual void setScreenSize(vector2U size) = 0;

		virtual ViewHandle getView() = 0;
		virtual uint32_t getViewImageIndex() = 0;
	};
}