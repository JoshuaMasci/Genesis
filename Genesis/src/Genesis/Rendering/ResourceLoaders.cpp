#include "ResourceLoaders.hpp"

using namespace Genesis;

#include <fstream>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

Mesh ObjLoader::loadMesh(RenderingBackend * backend, string mesh_file_path)
{
	struct TexturedVertex
	{
		vector3F pos;
		vector3F normal;
		vector2F uv;
	};

	Mesh mesh;

	tinyobj::attrib_t attrib;
	vector<tinyobj::shape_t> shapes;
	vector<tinyobj::material_t> materials;
	string warn, err;

	if (tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, mesh_file_path.c_str()))
	{
		vector<TexturedVertex> vertices;
		vector<uint32_t> indices;

		for (const auto& shape : shapes)
		{
			size_t index_offset = 0;
			for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++)
			{
				int fv = shape.mesh.num_face_vertices[f];

				// Loop over vertices in the face
				for (size_t v = 0; v < fv; v++)
				{
					// access to vertex
					tinyobj::index_t idx = shape.mesh.indices[index_offset + v];
					vector3F position = vector3F(attrib.vertices[3 * idx.vertex_index + 0], attrib.vertices[3 * idx.vertex_index + 1], attrib.vertices[3 * idx.vertex_index + 2]);
					vector3F normal = vector3F(attrib.normals[3 * idx.normal_index + 0], attrib.normals[3 * idx.normal_index + 1], attrib.normals[3 * idx.normal_index + 2]);
					vector2F uv = vector2F(attrib.texcoords[2 * idx.texcoord_index + 0], -attrib.texcoords[2 * idx.texcoord_index + 1]);

					TexturedVertex vertex = { position, normal, uv };

					vertices.push_back(vertex);
					indices.push_back((uint32_t)indices.size());
				}
				index_offset += fv;
			}
		}

		VertexInputDescription vertex_description
		({
			{"in_position", VertexElementType::float_3},
			{"in_normal", VertexElementType::float_3},
			{"in_uv", VertexElementType::float_2}
		});

		mesh.vertex_buffer = backend->createVertexBuffer(vertices.data(), sizeof(TexturedVertex) * vertices.size(), vertex_description);
		mesh.index_buffer = backend->createIndexBuffer(indices.data(), (uint32_t)indices.size(), IndexType::uint32);
	}
	else
	{
		printf("Error: Can't load Mesh: %s\n", mesh_file_path.c_str());
	}

	return mesh;
}

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#include <stb_image.h>

Texture PngLoader::loadTexture(RenderingBackend * backend, string texture_file_path)
{
	int width, height, tex_channels;
	uint8_t* data = stbi_load(texture_file_path.c_str(), &width, &height, &tex_channels, STBI_rgb_alpha);
	uint64_t data_size = width * height * STBI_rgb_alpha;

	if (data == NULL)
	{
		printf("Error: Can't load Texture: %s\n", texture_file_path.c_str());
		return nullptr;
	}

	Texture texture = backend->createTexture(vector2U((uint32_t)width, (uint32_t)height), (void*)data, data_size);
	stbi_image_free(data);
	return texture;
}

Shader ShaderLoader::loadShader(RenderingBackend* backend, string vert_file_path, string frag_file_path)
{
	string vert_data = "";
	string frag_data = "";

	std::ifstream vert_file(vert_file_path, std::ios::ate | std::ios::binary);
	if (vert_file.is_open())
	{
		size_t fileSize = (size_t)vert_file.tellg();
		vert_file.seekg(0);
		vert_data.resize(fileSize);
		vert_file.read(vert_data.data(), vert_data.size());
		vert_file.close();
	}

	std::ifstream frag_file(frag_file_path, std::ios::ate | std::ios::binary);
	if (frag_file.is_open())
	{
		size_t fileSize = (size_t)frag_file.tellg();
		frag_file.seekg(0);
		frag_data.resize(fileSize);
		frag_file.read(frag_data.data(), frag_data.size());
		frag_file.close();
	}

	return backend->createShader(vert_data, frag_data);
}

Shader ShaderLoader::loadShaderSingle(RenderingBackend* backend, string shader_file_path)
{
	return ShaderLoader::loadShader(backend, shader_file_path + ".vert.spv", shader_file_path + ".frag.spv");
}
