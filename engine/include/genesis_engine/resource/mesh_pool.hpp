#pragma once

#include "genesis_engine/resource/resource_pool.hpp"
#include "genesis_engine/resource/mesh.hpp"
#include "genesis_engine/LegacyBackend/LegacyBackend.hpp"

namespace genesis
{
	class MeshPool : public ResourcePool<string, Mesh>
	{
	public:
		MeshPool(LegacyBackend* backend);

	protected:
		LegacyBackend* backend = nullptr;
		virtual shared_ptr<Mesh> loadResource(const string& key) override;
	};
}