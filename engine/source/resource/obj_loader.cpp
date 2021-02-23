#include "genesis_engine/resource/obj_loader.hpp"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include "genesis_engine/resource/vertex_structs.hpp"

namespace genesis
{
	MeshStruct ObjLoader::loadMesh(LegacyBackend* backend, const string& filename)
	{
		tinyobj::attrib_t attrib;
		vector<tinyobj::shape_t> shapes;
		vector<tinyobj::material_t> materials;
		string warn, err;

		GENESIS_ENGINE_ASSERT((tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filename.c_str())), "Can't load Mesh");

		MeshStruct return_mesh = {};
		vector<MeshVertex> vertices;
		vector<uint32_t> indices;

		for (const auto& shape : shapes)
		{
			size_t index_offset = 0;

			//TODO
			vec3f min_position = vec3f(10000.0f);
			vec3f max_position = vec3f(-10000.0f);

			for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++)
			{
				size_t fv = shape.mesh.num_face_vertices[f];

				// Loop over vertices in the face
				for (size_t v = 0; v < fv; v++)
				{
					// access to vertices[i]
					tinyobj::index_t idx = shape.mesh.indices[index_offset + v];
					vec3f position = vec3f(attrib.vertices[3 * idx.vertex_index + 0], attrib.vertices[3 * idx.vertex_index + 1], attrib.vertices[3 * idx.vertex_index + 2]);
					vec3f normal = vec3f(attrib.normals[3 * idx.normal_index + 0], attrib.normals[3 * idx.normal_index + 1], attrib.normals[3 * idx.normal_index + 2]);
					vec2f uv = vec2f(attrib.texcoords[2 * idx.texcoord_index + 0], -attrib.texcoords[2 * idx.texcoord_index + 1]);

					MeshVertex vertex;
					vertex.position = position;
					vertex.normal = normal;
					vertex.uv = uv;

					vertices.push_back(vertex);
					indices.push_back((uint32_t)indices.size());

					min_position = glm::min(min_position, position);
					max_position = glm::max(max_position, position);
				}


				index_offset += fv;
			}

			//MeshPrimitive primitive;
			//primitive.first_index = index_offset;
			//primitive.index_count = shape.mesh.indices.size();
			//primitive.bounding_box = { min_position, max_position };
			//return_mesh.primitives.push_back(primitive);
		}

		//Calculate Tangent and Bitangent
		for (size_t i = 0; i < indices.size(); i += 3)
		{
			uint32_t index_1 = indices[i + 0];
			uint32_t index_2 = indices[i + 1];
			uint32_t index_3 = indices[i + 2];

			vec3f edge_1 = vertices[index_2].position - vertices[index_1].position;
			vec3f edge_2 = vertices[index_3].position - vertices[index_1].position;

			vec2f uv_1 = vertices[index_2].uv - vertices[index_1].uv;
			vec2f uv_2 = vertices[index_3].uv - vertices[index_1].uv;

			float factor = 1.0f / ((uv_1.x * uv_2.y) - (uv_2.x * uv_1.y));

			vec3f tangent;
			tangent.x = factor * ((uv_2.y * edge_1.x) - (uv_1.y * edge_2.x));
			tangent.y = factor * ((uv_2.y * edge_1.y) - (uv_1.y * edge_2.y));
			tangent.z = factor * ((uv_2.y * edge_1.z) - (uv_1.y * edge_2.z));
			tangent = glm::normalize(tangent);

			vertices[index_1].tangent = tangent;
			vertices[index_2].tangent = tangent;
			vertices[index_3].tangent = tangent;

			vertices[index_1].bitangent = glm::normalize(glm::cross(vertices[index_1].tangent, vertices[index_1].normal));
			vertices[index_2].bitangent = glm::normalize(glm::cross(vertices[index_2].tangent, vertices[index_2].normal));
			vertices[index_3].bitangent = glm::normalize(glm::cross(vertices[index_3].tangent, vertices[index_3].normal));
		}

		VertexInputDescriptionCreateInfo create_info = {};
		vector<VertexElementType> elements =
		{
			VertexElementType::float_3,
			VertexElementType::float_3,
			VertexElementType::float_3,
			VertexElementType::float_3,
			VertexElementType::float_2
		};
		create_info.input_elements = elements.data();
		create_info.input_elements_count = (uint32_t)elements.size();

		return_mesh.vertex_buffer = backend->createVertexBuffer(vertices.data(), vertices.size() * sizeof(MeshVertex), create_info);
		return_mesh.index_buffer = backend->createIndexBuffer(indices.data(), indices.size() * sizeof(uint32_t), IndexType::uint32);
		return_mesh.index_count = (uint32_t)indices.size();

		return return_mesh;
	}
}