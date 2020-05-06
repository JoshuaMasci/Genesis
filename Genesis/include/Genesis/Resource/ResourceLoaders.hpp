#pragma once

#include "Genesis/RenderingBackend/RenderingTypes.hpp"
#include "Genesis/Resource/Material.hpp"
#include "Genesis/RenderingBackend/RenderingBackend.hpp"

namespace Genesis
{
	class ShaderLoader
	{
	public:
		static Shader loadShader(RenderingBackend* backend, string vert_file_path, string frag_file_path);
		static Shader loadShaderSingle(RenderingBackend* backend, string shader_file_path);
	};

	class ObjLoader
	{
	public:
		struct Vertex
		{
			vector3F pos;
			vector2F uv;
			vector3F normal;
			vector3F tangent;
			vector3F bitangent;
		};

		static void loadMesh(RenderingBackend* backend, string mesh_file_path, StaticBuffer& vertex_buffer, StaticBuffer& index_buffer, uint32_t& index_count, float& frustum_sphere_radius);
		static void loadMesh(string mesh_file_path, vector<Vertex>& vertices, vector<uint32_t>& indices, float& frustum_sphere_radius);
	};

	class PngLoader
	{
	public:
		static Texture loadTexture(RenderingBackend* backend, string texture_file_path);

		static uint8_t* loadTexture(const string& texture_file_path, vector2I& size, int32_t& channels);
		static void unloadTexture(uint8_t* data);
	};

	class MaterialLoader
	{
	public:
		static MaterialDescription loadMaterial(const string& material_file_path);
	};
}