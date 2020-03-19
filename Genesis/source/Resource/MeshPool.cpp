#include "Genesis/Resource/MeshPool.hpp"

#include "Genesis/Rendering/ResourceLoaders.hpp"

using namespace Genesis;

MeshPool::MeshPool(RenderingBackend* backend)
{
	this->backend = backend;

	VertexElementType vertex_input[] = { VertexElementType::float_3, VertexElementType::float_2, VertexElementType::float_3, VertexElementType::float_3, VertexElementType::float_3 };
	VertexInputDescriptionCreateInfo vertex_input_create_info = {};
	vertex_input_create_info.input_elements = (VertexElementType*)&vertex_input;
	vertex_input_create_info.input_elements_count = _countof(vertex_input);
	this->vertex_input = this->backend->createVertexInputDescription(vertex_input_create_info);
}

void MeshPool::loadResource(string key)
{
	Mesh mesh;
	ObjLoader::loadMesh(this->backend, key, mesh.vertex_buffer, mesh.index_buffer, mesh.index_count, mesh.frustum_sphere_radius);
	mesh.index_type = IndexType::uint32;
	mesh.vertex_description = &vertex_input;

	this->resources[key].resource = mesh;
	this->resources[key].using_count = 0;
}

void MeshPool::unloadResource(string key)
{
	ResourceInternal resource = this->resources[key];
	this->backend->destroyStaticBuffer(resource.resource.vertex_buffer);
	this->backend->destroyStaticBuffer(resource.resource.index_buffer);
	this->resources.erase(key);
}
