#pragma once

#include "Genesis/Core/Types.hpp"
#include "Genesis/Rendering/RenderingBackend.hpp"

#include "Genesis/Entity.hpp"

namespace Genesis
{
	struct Model
	{
		Model(string mesh, string textrue)
		{
			this->mesh = mesh;
			this->texture = texture;
		};
		string mesh;
		string texture;
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
		Buffer* vertices = nullptr;
		Buffer* indices = nullptr;
		uint32_t indices_count = 0;
	};

	class Renderer
	{
	public:
		Renderer(RenderingBackend* backend);
		~Renderer();

		void drawFrame(EntityRegistry& EntityRegistry);

		//Temp resource stuff
		void loadMesh(string mesh_file);
		TextureIndex loadTexture(string texture_file);

	private:
		//Lifetime of the Backend is longer than the Renderer
		RenderingBackend* backend;

		//Temp resource stuff
		map<string, Mesh> loaded_meshes;

		TextureIndex texture = NULL_INDEX;
	};
}