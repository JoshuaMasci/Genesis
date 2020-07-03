#pragma once

#include "Genesis/ECS/EntityRegistry.hpp"
#include <Genesis/LegacyBackend/LegacyBackend.hpp>

namespace Genesis
{
	class SceneViewWindow
	{
	public:
		SceneViewWindow(LegacyBackend* legacy_backend);
		~SceneViewWindow();
		void drawWindow(EntityWorld* world);

	private:
		LegacyBackend* legacy_backend;
	};
}