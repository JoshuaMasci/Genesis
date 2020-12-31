#include "Genesis/Game/ChunkMeshGenerator.hpp"

#include "Genesis/Resource/VertexStructs.hpp"

namespace Genesis
{
	ChunkMeshGenerator::ChunkMeshGenerator(Genesis::LegacyBackend* backend)
	{
		this->backend = backend;
	}

	enum class BlockFace
	{
		x_p = 0, //Left
		x_n = 1, //Right
		y_p = 2, //Top
		y_n = 3, //Bottom
		z_p = 4, //Front
		z_n = 5, //Back
		Count = 6,
	};

	Genesis::vector3F cube_verts[8] =
	{
		Genesis::vector3F(0.5f,  0.5f,  0.5f),
		Genesis::vector3F(0.5f,  0.5f, -0.5f),
		Genesis::vector3F(0.5f, -0.5f,  0.5f),
		Genesis::vector3F(0.5f, -0.5f, -0.5f),
		Genesis::vector3F(-0.5f,  0.5f,  0.5f),
		Genesis::vector3F(-0.5f,  0.5f, -0.5f),
		Genesis::vector3F(-0.5f, -0.5f,  0.5f),
		Genesis::vector3F(-0.5f, -0.5f, -0.5f),
	};

	uint16_t cube_faces[6][4] =
	{
		{0,2,3,1},
		{4,5,7,6},
		{0,1,5,4},
		{2,6,7,3},
		{0,4,6,2},
		{1,3,7,5},
	};

	vector3I face_check[] =
	{
		vector3I(1, 0, 0),
		vector3I(-1, 0, 0),
		vector3I(0, 1, 0),
		vector3I(0, -1, 0),
		vector3I(0, 0, 1),
		vector3I(0, 0, -1),
	};

	void addFace(vector<MeshVertex>& vertices, vector<uint32_t>& indices, const vector3F& postion, const vector3F& normal, uint32_t face_index)
	{
		//TODO tangent/bitangent
		MeshVertex vertex_1 = { cube_verts[cube_faces[face_index][0]] + postion, normal, normal, normal, vector2F(0.0f, 0.0f) };
		MeshVertex vertex_2 = { cube_verts[cube_faces[face_index][1]] + postion, normal, normal, normal, vector2F(1.0f, 0.0f) };
		MeshVertex vertex_3 = { cube_verts[cube_faces[face_index][2]] + postion, normal, normal, normal, vector2F(1.0f, 1.0f) };
		MeshVertex vertex_4 = { cube_verts[cube_faces[face_index][3]] + postion, normal, normal, normal, vector2F(0.0f, 1.0f) };

		size_t vertex_offset = vertices.size();
		vertices.push_back(vertex_1);
		vertices.push_back(vertex_2);
		vertices.push_back(vertex_3);
		vertices.push_back(vertex_4);

		indices.push_back(vertex_offset + 0);
		indices.push_back(vertex_offset + 1);
		indices.push_back(vertex_offset + 2);

		indices.push_back(vertex_offset + 2);
		indices.push_back(vertex_offset + 3);
		indices.push_back(vertex_offset + 0);
	}

	Genesis::Mesh* ChunkMeshGenerator::generateMesh(const DefaultChunk& chunk)
	{
		MeshStruct return_mesh = {};
		vector<MeshVertex> vertices;
		vector<uint32_t> indices;

		vector3I chunk_size = chunk.getSize();

		for (uint32_t x = 0; x < chunk_size.x; x++)
		{
			for (uint32_t y = 0; y < chunk_size.y; y++)
			{
				for (uint32_t z = 0; z < chunk_size.z; z++)
				{
					vector3I block_pos(x, y, z);
					if (chunk.hasBlock(block_pos))
					{
						for (uint32_t face_index = 0; face_index < (uint32_t)BlockFace::Count; face_index++)
						{
							if (!chunk.hasBlock(block_pos + face_check[face_index]))
							{
								addFace(vertices, indices, (vector3F)block_pos, (vector3F)face_check[face_index], face_index);
							}
						}
					}
				}
			}
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

		return new Mesh("", this->backend, return_mesh);
	}
}