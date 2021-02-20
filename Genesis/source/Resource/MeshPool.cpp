#include "Genesis/Resource/MeshPool.hpp"

#include "Genesis/Resource/ObjLoader.hpp"

namespace Genesis
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