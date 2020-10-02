#include "Genesis/Resource/MeshPool.hpp"

#include "Genesis/Resource/ObjLoader.hpp"

namespace Genesis
{
	MeshPool::MeshPool(LegacyBackend* backend)
	{
		this->backend = backend;
	}

	shared_ptr<MeshResource> MeshPool::loadResource(const string& key)
	{
		Mesh mesh = ObjLoader::loadMesh(this->backend, key);
		return std::make_shared<MeshResource>(key, this->backend, mesh);
	}
}