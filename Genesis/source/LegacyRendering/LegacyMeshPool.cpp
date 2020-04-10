#include "Genesis/LegacyRendering/LegacyMeshPool.hpp"

#include "Genesis/Resource/ResourceLoaders.hpp"

using namespace Genesis;

LegacyMeshPool::LegacyMeshPool(LegacyBackend* backend)
{
	this->legacy_backend = backend;

	//                      Position                    UV                          Normal                      Tangent                     Bitangent
	this->vertex_inputs = { VertexElementType::float_3, VertexElementType::float_2, VertexElementType::float_3, VertexElementType::float_3, VertexElementType::float_3 };
}

LegacyMeshPool::~LegacyMeshPool()
{
	for (auto resource : this->resources)
	{
		this->unloadResource(resource.first);
	}
}

void LegacyMeshPool::loadResource(string key)
{
	vector<ObjLoader::Vertex> vertices;
	vector<uint32_t> indices;
	float radius;

	ObjLoader::loadMesh(key, vertices, indices, radius);

	LegacyMesh mesh = {};
	mesh.vertex_buffer = this->legacy_backend->createVertexBuffer(vertices.data(), vertices.size() * sizeof(ObjLoader::Vertex), {this->vertex_inputs.data(), (uint32_t)this->vertex_inputs.size()});
	mesh.index_buffer = this->legacy_backend->createIndexBuffer(indices.data(), indices.size() * sizeof(uint32_t), IndexType::uint32);
	mesh.index_count = (uint32_t)indices.size();
	mesh.frustum_sphere_radius = radius;

	this->resources[key].resource = mesh;
	this->resources[key].using_count = 0;
}

void LegacyMeshPool::unloadResource(string key)
{
	ResourceInternal resource = this->resources[key];
	this->legacy_backend->destoryVertexBuffer(resource.resource.vertex_buffer);
	this->legacy_backend->destoryIndexBuffer(resource.resource.index_buffer);
}
