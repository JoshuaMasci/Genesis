#pragma once

#include "Genesis/Core/Types.hpp"

namespace Genesis
{
	class View
	{
	public:
		virtual ~View() {};

		virtual void startView() = 0;
		virtual void endView() = 0;
	};
}