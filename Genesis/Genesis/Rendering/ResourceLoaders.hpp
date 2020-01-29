#pragma once

#include "Genesis/Core/Types.hpp"
#include "Genesis/Rendering/RenderingTypes.hpp"
#include "Genesis/Rendering/RenderingBackend.hpp"
#include "Genesis/Rendering/Skeleton.hpp"

namespace Genesis
{
	struct MeshTemp
	{
		VertexBuffer vertex_buffer;
		IndexBuffer index_buffer;
		uint32_t index_count;
	};

	class ObjLoader
	{
	public:
		static MeshTemp loadMesh(RenderingBackend* backend, string mesh_file_path);
		static MeshTemp loadMesh_CalcTangent(RenderingBackend* backend, string mesh_file_path);

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

	/*class GLTF_Loader
	{
	public:
		static void loadGLTF(RenderingBackend* backend, string file_path);
	};*/

	class AssimpLoader
	{
	public:
		static MeshTemp loadMesh(RenderingBackend* backend, string file_path);
		static Skeleton* loadSkeleton(RenderingBackend* backend, string file_path);
	};
}