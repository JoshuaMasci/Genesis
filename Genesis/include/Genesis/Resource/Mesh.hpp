#pragma once

#include "Genesis/RenderingBackend/RenderingTypes.hpp"
#include "Genesis/Rendering/BoundingBox.hpp"

namespace Genesis
{
	struct MeshVertex
	{
		vector3F position;
		vector3F normal;
		vector3F tangent;
		vector3F bitangent;
		vector2F uv;
	};

	struct MeshVertexAnimated
	{
		vector3F position;
		vector3F normal;
		vector3F tangent;
		vector3F bitangent;
		vector2F uv;

		vector4F joints;
		vector4F weights;
	};


	struct MeshPrimitive
	{
		uint32_t first_index;
		uint32_t index_count;
		uint32_t vertex_count;
		BoundingBox bounding_box;
	};

	struct Mesh
	{
		VertexBuffer vertex_buffer;
		IndexBuffer index_buffer;
		uint32_t index_count;

		BoundingBox bounding_box;

		bool has_weights = false;
	};

	struct MeshUtils
	{
		void loadMeshFromFile(const string& file);
	};
}