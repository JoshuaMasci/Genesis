#pragma once

#include "Genesis/Core/Types.hpp"
#include "Genesis/Rendering/RenderingBackend.hpp"

#include "Genesis/Entity.hpp"

namespace Genesis
{
	//TEMP
	struct SecondView
	{

	};


	struct Model
	{
		Model(string mesh, string texture)
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
		BufferIndex vertices = NULL_INDEX;
		BufferIndex indices = NULL_INDEX;
		uint32_t indices_count = 0;
	};

	class Renderer
	{
	public:
		Renderer(RenderingBackend* backend);
		~Renderer();

		void drawFrame(EntityRegistry& entity_registry, EntityId camera_entity);

		//Temp resource stuff
		void loadMesh(string mesh_file);
		void loadTexture(string texture_file);

	private:
		//Lifetime of the Backend is longer than the Renderer
		//No deleteing please
		RenderingBackend* backend;

		//Temp resource stuff
		map<string, Mesh> loaded_meshes;
		map<string, TextureIndex> loaded_textures;

		//SecondCam
		ViewIndex view;
	};
}