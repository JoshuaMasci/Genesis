#pragma once

#include "Genesis/Core/Types.hpp"

#include "Genesis/Entity.hpp"
#include "Genesis/Job/JobSystem.hpp"
#include "Genesis/Platform/Window.hpp"

namespace Genesis
{
	//Prototypes
	class TexturedMesh;
	class ShaderProgram;

	struct TexturedModel
	{
		TexturedModel(uint16_t mesh, uint16_t texture, uint16_t shader)
		{
			this->mesh = mesh;
			this->texture = texture;
			this->shader = shader;
		}

		uint16_t mesh;
		uint16_t texture;
		uint16_t shader;
	};

	struct ShaderSet
	{
		string vert;
		string frag;
	};

	class RenderSystem
	{
	public:
		RenderSystem(Window* window);
		~RenderSystem();

		void drawFrame(EntityRegistry& registry, JobSystem& job_system, double delta_time);

	private:
		Window* window;
		void* context;

		vector<TexturedMesh*> meshes;
		vector<uint32_t> textures;
		vector<ShaderProgram*> shaders;
	};
}