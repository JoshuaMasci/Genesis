#pragma once

#include "Genesis/Resource/Resource.hpp"
#include "Genesis/LegacyBackend/LegacyBackend.hpp"
#include "Genesis/Resource/Mesh.hpp"

namespace Genesis
{
	class MeshResource : public Resource
	{
	protected:
		LegacyBackend* backend;

	public:
		MeshResource(const string& file_path, LegacyBackend* backend, const Mesh& mesh)
			:Resource(file_path), backend(backend), vertex_buffer(mesh.vertex_buffer), index_buffer(mesh.index_buffer), index_count(mesh.index_count), bounding_box(mesh.bounding_box){};

		~MeshResource()
		{
			this->backend->destoryVertexBuffer(this->vertex_buffer);
			this->backend->destoryIndexBuffer(this->index_buffer);
		}

		const VertexBuffer vertex_buffer = nullptr;
		const IndexBuffer index_buffer = nullptr;
		const uint32_t index_count;
		const BoundingBox bounding_box;
	};
}