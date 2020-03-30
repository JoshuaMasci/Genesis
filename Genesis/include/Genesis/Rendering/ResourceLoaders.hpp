#pragma once

#include "Genesis/Core/Types.hpp"
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
		static void loadMesh(RenderingBackend* backend, string mesh_file_path, StaticBuffer& vertex_buffer, StaticBuffer& index_buffer, uint32_t& index_count, float& frustum_sphere_radius);
	};

	class PngLoader
	{
	public:
		static Texture loadTexture(RenderingBackend* backend, string texture_file_path);
	};

	class MaterialLoader
	{
	public:
		static Material loadMaterial(RenderingBackend* backend, string material_file_path);
	};
}