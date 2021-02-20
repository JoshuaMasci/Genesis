#pragma once

#include <unordered_map>
#include "genesis_engine/platform/FileSystem.hpp"
#include "genesis_engine/LegacyBackend/LegacyBackend.hpp"

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