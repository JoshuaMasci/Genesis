#pragma once

#include "Genesis/LegacyRendering/LegacyMesh.hpp"
#include "Genesis/Resource/ResourcePool.hpp"
#include "Genesis/LegacyBackend/LegacyBackend.hpp"

namespace Genesis
{
	class LegacyMeshPool : public ResourcePool<string, LegacyMesh>
	{
	public:
		LegacyMeshPool(LegacyBackend* backend);
		virtual ~LegacyMeshPool();

	protected:
		virtual void loadResource(string key) override;
		virtual void unloadResource(string key) override;

		LegacyBackend* legacy_backend;
		vector<VertexElementType> vertex_inputs;
	};
}