#pragma once

#include <unordered_map>
#include "Genesis/Platform/FileSystem.hpp"
#include "Genesis/LegacyBackend/LegacyBackend.hpp"

namespace Genesis
{
	class RenderStatisticsWindow
	{
	private:
		LegacyBackend* backend = nullptr;

	public:
		RenderStatisticsWindow(LegacyBackend* backend);
		void draw(TimeStep time_step);
	};
}