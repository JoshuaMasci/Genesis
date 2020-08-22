#include "Genesis/Resource/MeshPool.hpp"

#include "Genesis/Resource/ObjLoader.hpp"

namespace Genesis
{
	MeshPool::MeshPool(LegacyBackend* backend)
	{
		this->backend = backend;
	}

	void MeshPool::loadResource(const string& key)
	{
		Mesh mesh = ObjLoader::loadMesh(this->backend, key);
		this->resources[key].resource = mesh;
		this->resources[key].using_count = 0;
	}

	void MeshPool::unloadResource(const string& key)
	{
		ResourceInternal resource = this->resources[key];
		this->backend->destoryVertexBuffer(resource.resource.vertex_buffer);
		this->backend->destoryIndexBuffer(resource.resource.index_buffer);
	}
}