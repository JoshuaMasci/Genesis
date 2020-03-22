#include "Genesis/Rendering/ResourceLoaders.hpp"

using namespace Genesis;

#include "Genesis/Debug/Assert.hpp"
#include "Genesis/Debug/Log.hpp"
#include <fstream>

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
		vert_data.assign((std::istreambuf_iterator<char>(vert_file)),(std::istreambuf_iterator<char>()));
		vert_file.close();
	}
	else
	{
		GENESIS_ENGINE_ERROR("Failed to open file: {}", vert_file_path);
	}

	std::ifstream frag_file(frag_file_path, std::ios::ate | std::ios::binary);
	if (frag_file.is_open())
	{
		size_t fileSize = (size_t)frag_file.tellg();
		frag_file.seekg(0);
		frag_data.resize(fileSize);
		frag_data.assign((std::istreambuf_iterator<char>(frag_file)), (std::istreambuf_iterator<char>()));
		frag_file.close();
	}
	else
	{
		GENESIS_ENGINE_ERROR("Failed to open file: {}", frag_file_path);
	}

	return backend->createShader(vert_data, frag_data);
}

Shader ShaderLoader::loadShaderSingle(RenderingBackend* backend, string shader_file_path)
{
	return ShaderLoader::loadShader(backend, shader_file_path + ".vert.spv", shader_file_path + ".frag.spv");
}

#define TINYOBJLOADER_IMPLEMENTATION

#include <tiny_obj_loader.h>
void Genesis::ObjLoader::loadMesh(RenderingBackend* backend, string mesh_file_path, StaticBuffer& vertex_buffer, StaticBuffer& index_buffer, uint32_t& index_count, float& frustum_sphere_radius)
{
	struct TexturedVertex
	{
		vector3F pos;
		vector2F uv;
		vector3F normal;
		vector3F tangent;
		vector3F bitangent;
	};

	tinyobj::attrib_t attrib;
	vector<tinyobj::shape_t> shapes;
	vector<tinyobj::material_t> materials;
	string warn, err;

	GENESIS_ENGINE_ASSERT_ERROR((tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, mesh_file_path.c_str())), "Can't load Mesh");
	
	vector<TexturedVertex> vertices;
	vector<uint32_t> indices;

	float distance = 0.0f;

	for (const auto& shape : shapes)
	{
		size_t index_offset = 0;
		for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++)
		{
			int fv = shape.mesh.num_face_vertices[f];

			// Loop over vertices in the face
			for (size_t v = 0; v < fv; v++)
			{
				// access to vertices[i]
				tinyobj::index_t idx = shape.mesh.indices[index_offset + v];
				vector3F position = vector3F(attrib.vertices[3 * idx.vertex_index + 0], attrib.vertices[3 * idx.vertex_index + 1], attrib.vertices[3 * idx.vertex_index + 2]);
				vector3F normal = vector3F(attrib.normals[3 * idx.normal_index + 0], attrib.normals[3 * idx.normal_index + 1], attrib.normals[3 * idx.normal_index + 2]);
				vector2F uv = vector2F(attrib.texcoords[2 * idx.texcoord_index + 0], -attrib.texcoords[2 * idx.texcoord_index + 1]);

				TexturedVertex vertex = { position, uv, normal};

				vertices.push_back(vertex);
				indices.push_back((uint32_t)indices.size());


				float pos_length = glm::length(position);
				if (pos_length > distance)
				{
					distance = pos_length;
				}
			}
			index_offset += fv;
		}
	}

	for (size_t i = 0; i < indices.size(); i += 3)
	{
		uint32_t index_1 = indices[i + 0];
		uint32_t index_2 = indices[i + 1];
		uint32_t index_3 = indices[i + 2];

		vector3F edge_1 = vertices[index_2].pos - vertices[index_1].pos;
		vector3F edge_2 = vertices[index_3].pos - vertices[index_1].pos;

		vector2F uv_1 = vertices[index_2].uv - vertices[index_1].uv;
		vector2F uv_2 = vertices[index_3].uv - vertices[index_1].uv;

		float factor = 1.0f / ((uv_1.x * uv_2.y) - (uv_2.x * uv_1.y));

		vector3F tangent;
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

	vertex_buffer = backend->createStaticBuffer(vertices.data(), sizeof(TexturedVertex) * vertices.size(), BufferUsage::Vertex_Buffer);
	index_buffer = backend->createStaticBuffer(indices.data(), sizeof(uint32_t) * indices.size(), BufferUsage::Index_Buffer);
	index_count = (uint32_t)indices.size();
	frustum_sphere_radius = distance;
}

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

Texture PngLoader::loadTexture(RenderingBackend * backend, string texture_file_path)
{
	int width, height, tex_channels;
	uint8_t* data = stbi_load(texture_file_path.c_str(), &width, &height, &tex_channels, STBI_rgb_alpha);
	uint64_t data_size = width * height * STBI_rgb_alpha;

	if (data == NULL)
	{
		GENESIS_ENGINE_WARN("Can't load Texture {}", texture_file_path);
		return nullptr;
	}

	Texture texture = backend->createTexture(vector2U((uint32_t)width, (uint32_t)height), (void*)data, data_size);
	stbi_image_free(data);
	return texture;
	return nullptr;
}

/*#include <jsoncons/json.hpp>

Material MaterialLoader::loadMaterial(RenderingBackend* backend, string material_file_path)
{
	jsoncons::json material_json = jsoncons::json::parse_file(material_file_path);

	Material material;
	MaterialValues values;

	if (material_json.has_key("Maps"))
	{
		jsoncons::json json_maps = material_json.at("Maps");
	
		if (json_maps.has_key("albedo_map"))
		{
			values.has_albedo_map = true;
			material.bindings.albedo_map = PngLoader::loadTexture(backend, json_maps["albedo_map"].as_string());
		}
		else
		{
			values.has_albedo_map = false;
		}

		if (json_maps.has_key("normal_map"))
		{
			values.has_normal_map = true;
			material.bindings.normal_map = PngLoader::loadTexture(backend, json_maps["normal_map"].as_string());
		}
		else
		{
			values.has_normal_map = false;
		}

		if (json_maps.has_key("height_map"))
		{
			values.has_height_map = true;
			material.bindings.height_map = PngLoader::loadTexture(backend, json_maps["height_map"].as_string());
		}
		else
		{
			values.has_height_map = false;
		}

		if (json_maps.has_key("metallic_map"))
		{
			values.has_metallic_map = true;
			material.bindings.metallic_map = PngLoader::loadTexture(backend, json_maps["metallic_map"].as_string());
		}
		else
		{
			values.has_metallic_map = false;
		}

		if (json_maps.has_key("roughness_map"))
		{
			values.has_roughness_map = true;
			material.bindings.roughness_map = PngLoader::loadTexture(backend, json_maps["roughness_map"].as_string());
		}
		else
		{
			values.has_roughness_map = false;
		}

		if (json_maps.has_key("occlusion_map"))
		{
			values.has_ambient_occlusion_map = true;
			material.bindings.occlusion_map = PngLoader::loadTexture(backend, json_maps["occlusion_map"].as_string());
		}
		else
		{
			values.has_ambient_occlusion_map = false;
		}
	}

	if (material_json.has_key("Values"))
	{
		jsoncons::json json_values = material_json.at("Values");
		//values.albedo = 
	}



	return material;
}*/
