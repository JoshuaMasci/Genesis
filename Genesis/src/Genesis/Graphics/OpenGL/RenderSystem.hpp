#pragma once

#include "Genesis/Core/Types.hpp"

#include "Genesis/Entity.hpp"
#include "Genesis/Job/JobSystem.hpp"
#include "Genesis/Platform/Window.hpp"

namespace Genesis
{
	//Prototypes
	class TexturedMesh;

	struct TexturedModel
	{
		uint16_t mesh;
		uint16_t texture;
		uint16_t shader;
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
	};
}