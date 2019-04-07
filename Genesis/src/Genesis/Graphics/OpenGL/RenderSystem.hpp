#pragma once

#include "Genesis/Core/Types.hpp"
#include "Genesis/Graphics/OpenGL/OpenGL_Include.hpp"
#include "Genesis/Graphics/OpenGL/TexturedMesh.hpp"

#include "Genesis/Entity.hpp"
#include "Genesis/Job/JobSystem.hpp"
#include "Genesis/Platform/Window.hpp"

namespace Genesis
{
	struct TexturedModel
	{
		uint16_t mesh;
		uint16_t texture;
	};

	class RenderSystem
	{
	public:
		RenderSystem(Window* window);
		~RenderSystem();

		void drawFrame(EntityRegistry& registry, JobSystem& job_system, double delta_time);

	private:
		Window* window;

		vector<TexturedMesh> meshes;
		vector<GLuint> textures;
	};
}