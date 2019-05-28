#pragma once

#include "Genesis/Rendering/RenderingBackend.hpp"

namespace Genesis
{
	struct Vertex
	{
		vector3F pos;
		vector3F color;
		vector2F texCoord;
	};

	class Renderer
	{
	public:
		Renderer(RenderingBackend* backend);
		~Renderer();

		void drawFrame();

	private:
		//Lifetime of the Backend is longer than the Renderer
		RenderingBackend* backend;

		//TEMP
		Buffer* cube_vertices = nullptr;
		Buffer* cube_indices = nullptr;
		uint32_t cube_indices_count = 0;

		vector<vector3F> positions;
	};
}