#pragma once

#include "Genesis/LegacyBackend/LegacyBackend.hpp"

namespace Genesis
{
	class World;
	class Node;

	class LegacyWorldRenderer
	{
	public:
		LegacyWorldRenderer(LegacyBackend* backend);
		~LegacyWorldRenderer();

		void drawWorld(Framebuffer framebuffer, vector2U framebuffer_size, World* world);

	protected:
		LegacyBackend* backend;

		void drawNode(Node* node);

		ShaderProgram pbr_program;
	};
}