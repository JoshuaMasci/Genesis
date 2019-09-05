#pragma once

#include "Genesis/Core/Types.hpp"
#include "Genesis/Rendering/RenderingBackend.hpp"

#include "Genesis/Entity.hpp"

namespace Genesis
{
	struct Model
	{
		VertexBufferHandle vertices = nullptr;
		IndexBufferHandle indices = nullptr;
		TextureHandle texture = nullptr;
		ShaderHandle shader = nullptr;
		UniformBufferHandle matrix = nullptr;
	};

	struct TexturedVertex
	{
		vector3F pos;
		vector3F normal;
		vector2F uv;

		bool operator==(const TexturedVertex& other) const
		{
			return pos == other.pos && normal == other.normal && uv == other.uv;
		}
	};

	struct Mesh
	{
		VertexBufferHandle vertices = nullptr;
		IndexBufferHandle indices = nullptr;
	};

	class Renderer
	{
	public:
		Renderer(RenderingBackend* backend);
		~Renderer();

		void drawFrame(EntityRegistry& entity_registry, EntityId camera_entity);
		void endFrame();

		//Temp resource stuff
		void loadMesh(string mesh_file);
		void loadTexture(string texture_file);
		void loadShader(string shader_file_base);

		Model createModel(string mesh, string texture, string shader);
		void destroyModel(Model& model);

	private:
		//Lifetime of the Backend is longer than the Renderer
		//No deleting please
		RenderingBackend* backend;

		//Resources
		map<string, Mesh> loaded_meshes;
		map<string, TextureHandle> loaded_textures;
		map<string, ShaderHandle> loaded_shaders;
	};
}