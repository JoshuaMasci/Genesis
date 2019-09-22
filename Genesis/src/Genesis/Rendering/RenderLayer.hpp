#pragma once

#include "Genesis/Core/VectorTypes.hpp"
#include "Genesis/Rendering/RenderingTypes.hpp"

namespace Genesis
{
	class RenderLayer
	{
	public:
		virtual void setWindowSize(vector2U size) = 0;
		virtual ViewHandle getView() = 0;
	};
}