#pragma once

#include "Genesis/Rendering/BaseWorldRenderer.hpp"
#include "Genesis/LegacyBackend/LegacyBackend.hpp"

#include "Genesis/Rendering/MeshRenderer.hpp"
#include "Genesis/Entity/World.hpp"
#include "Genesis/Rendering/Frustum.hpp"

namespace Genesis
{
	class LegacyWorldRenderer : public BaseWorldRenderer
	{
	public:
		LegacyWorldRenderer(LegacyBackend* backend);
		~LegacyWorldRenderer();

		virtual void drawWorld(World* world, vector2U size) override;

	protected: 
		LegacyBackend* legacy_backend;

		vector<MeshRenderer*> renderers;
	};
}