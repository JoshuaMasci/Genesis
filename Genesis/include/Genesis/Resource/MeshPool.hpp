#pragma once

#include "Genesis/Resource/ResourcePool.hpp"
#include "Genesis/Resource/Mesh.hpp"
#include "Genesis/LegacyBackend/LegacyBackend.hpp"

#include "Genesis/Resource/MeshResource.hpp"

namespace Genesis
{
	class MeshPool : public ResourcePoolNew<string, MeshResource>
	{
	public:
		MeshPool(LegacyBackend* backend);

	protected:
		LegacyBackend* backend = nullptr;
		virtual shared_ptr<MeshResource> loadResource(const string& key) override;
	};
}