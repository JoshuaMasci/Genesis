#pragma once

#include "Genesis/Core/Types.hpp"
#include "Genesis/Rendering/RenderingTypes.hpp"
#include "Genesis/Rendering/RenderingBackend.hpp"

namespace Genesis
{
	struct Mesh
	{
		VertexBuffer vertex_buffer;
		IndexBuffer index_buffer;
	};

	class ObjLoader
	{
	public:
		static Mesh loadMesh(RenderingBackend* backend, string mesh_file_path);
		static Mesh loadMesh_CalcTangent(RenderingBackend* backend, string mesh_file_path);

	};

	class PngLoader
	{
	public:
		static Texture loadTexture(RenderingBackend* backend, string texture_file_path);
	};

	class ShaderLoader
	{
	public:
		static Shader loadShader(RenderingBackend* backend, string vert_file_path, string frag_file_path);
		static Shader loadShaderSingle(RenderingBackend* backend, string shader_file_path);
	};
}