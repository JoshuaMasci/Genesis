#pragma once

#include "Genesis/Resource/ResourcePool.hpp"
#include "Genesis/Resource/Mesh.hpp"
#include "Genesis/LegacyBackend/LegacyBackend.hpp"

#include "Genesis/Resource/Mesh.hpp"

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