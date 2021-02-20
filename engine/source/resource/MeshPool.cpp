#include "genesis_engine/resource/MeshPool.hpp"

#include "genesis_engine/resource/ObjLoader.hpp"

namespace genesis_engine
{
	MeshPool::MeshPool(LegacyBackend* backend)
	{
		this->backend = backend;
	}

	shared_ptr<Mesh> MeshPool::loadResource(const string& key)
	{
		MeshStruct mesh = ObjLoader::loadMesh(this->backend, key);
		return std::make_shared<Mesh>(key, this->backend, mesh);
	}
}