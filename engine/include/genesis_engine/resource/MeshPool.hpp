#pragma once

#include "genesis_engine/resource/ResourcePool.hpp"
#include "genesis_engine/resource/Mesh.hpp"
#include "genesis_engine/LegacyBackend/LegacyBackend.hpp"

#include "genesis_engine/resource/Mesh.hpp"

namespace Genesis
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