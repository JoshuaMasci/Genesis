#pragma once

#include "Genesis/Resource/ResourcePool.hpp"
#include "Genesis/Resource/Mesh.hpp"
#include "Genesis/LegacyBackend/LegacyBackend.hpp"

namespace Genesis
{
	class MeshPool : public ResourcePool<string, Mesh*>
	{
	public:
		MeshPool(LegacyBackend* backend);

	protected:
		LegacyBackend* backend = nullptr;

		virtual void loadResource(const string& key) override;
		virtual void unloadResource(const string& key) override;
	};
}