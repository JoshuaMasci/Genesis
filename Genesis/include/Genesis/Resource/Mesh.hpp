#pragma once

#include "Genesis/Resource/Resource.hpp"
#include "Genesis/LegacyBackend/LegacyBackend.hpp"
#include "Genesis/Rendering/BoundingBox.hpp"

namespace Genesis
{
	struct MeshStruct
	{
		VertexBuffer vertex_buffer;
		IndexBuffer index_buffer;
		uint32_t index_count;
		BoundingBox bounding_box;
	};

	class Mesh : public Resource
	{
	protected:
		LegacyBackend* backend;

	public:
		Mesh(const string& file_path, LegacyBackend* backend, const MeshStruct& mesh)
			:Resource(file_path), backend(backend), vertex_buffer(mesh.vertex_buffer), index_buffer(mesh.index_buffer), index_count(mesh.index_count), bounding_box(mesh.bounding_box){};

		~Mesh()
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