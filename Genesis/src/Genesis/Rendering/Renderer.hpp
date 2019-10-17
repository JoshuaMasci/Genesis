#pragma once

#include "Genesis/Core/Types.hpp"
#include "Genesis/Rendering/RenderingBackend.hpp"
#include "Genesis/Rendering/ResourceLoaders.hpp"

#include "Genesis/Entity.hpp"

namespace Genesis
{
	struct Model
	{
		string mesh;
		string texture;
		string shader;
	};

	class Renderer
	{
	public:
		Renderer(RenderingBackend* backend);
		~Renderer();

		void drawFrame(EntityRegistry& entity_registry, EntityId camera_entity);

		View view;
		vector3F ambient_light = vector3F(0.4f);


	private:
		//Lifetime of the Backend is longer than the Renderer
		//No deleting please
		RenderingBackend* backend;

		//Resources
		map<string, Mesh> loaded_meshes;
		map<string, Texture> loaded_textures;
		map<string, Shader> loaded_shaders;

		vector2U view_size;
	};
}