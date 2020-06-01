#pragma once

#include "Genesis/RenderingBackend/RenderingTypes.hpp"

#include "Genesis/Rendering/BoundingBox.hpp"

namespace Genesis
{
	struct PbrMaterial;

	struct PbrMeshVertex
	{
		vector3F position;
		vector3F normal;
		vector2F uv0;
		vector2F uv1;
		vector4F joint0;
		vector4F weight0;
	};

	struct PbrMeshPrimitive
	{
		uint32_t first_index;
		uint32_t index_count;
		uint32_t vertex_count;
		uint32_t material_index;
		BoundingBox bounding_box;

		PbrMaterial* temp_material_ptr;
	};

	struct PbrMesh
	{
		VertexBuffer vertex_buffer;
		IndexBuffer index_buffer;
		vector<PbrMeshPrimitive> primitives;
		BoundingBox bounding_box;
	};
}